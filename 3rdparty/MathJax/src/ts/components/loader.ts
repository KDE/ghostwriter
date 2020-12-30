/*************************************************************
 *
 *  Copyright (c) 2018 The MathJax Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**
 * @fileoverview  A dynamic loader for loading MathJax components based
 *                on a user configuration, while handling timing of
 *                dependencies properly
 *
 * @author dpvc@mathjax.org (Davide Cervone)
 */

import {MathJax as MJGlobal, MathJaxObject as MJObject, MathJaxLibrary,
        MathJaxConfig as MJConfig, combineWithMathJax, combineDefaults} from './global.js';

import {Package, PackageError, PackageReady, PackageFailed} from './package.js';
export {Package, PackageError, PackageReady, PackageFailed} from './package.js';
export {MathJaxLibrary} from './global.js';

/*
 * The current directory (for webpack), and the browser document (if any)
 */
declare var __dirname: string;
declare var document: Document;

/**
 * Update the configuration structure to include the loader configuration
 */
export interface MathJaxConfig extends MJConfig {
  loader?: {
    paths?: {[name: string]: string};          // The path prefixes for use in locations
    source?: {[name: string]: string};         // The URLs for the extensions, e.g., tex: [mathjax]/input/tex.js
    dependencies?: {[name: string]: string[]}; // The dependencies for each package
    provides?: {[name: string]: string[]};     // The sub-packages provided by each package
    load?: string[];                           // The packages to load (found in locations or [mathjax]/name])
    ready?: PackageReady;                      // A function to call when MathJax is ready
    failed?: PackageFailed;                    // A function to call when MathJax fails to load
    require?: (url: string) => any;            // A function for loading URLs
    [name: string]: any;                       // Other configuration blocks
  };
}

/**
 * Update the MathJax object to inclide the loader information
 */
export interface MathJaxObject extends MJObject {
  _: MathJaxLibrary;
  config: MathJaxConfig;
  loader: {
    ready: (...names: string[]) => Promise<string[]>; // Get a promise for when all the named packages are loaded
    load: (...names: string[]) => Promise<string>;    // Load the packages and return a promise for when ready
    preLoad: (...names: string[]) => void;            // Indicate that packages are already loaded by hand
    defaultReady: () => void;                         // The function performed when all packages are loaded
    getRoot: () => string;                            // Find the root URL for the MathJax files
  };
  startup?: any;
}

/**
 * The implementation of the dynamic loader
 */
export namespace Loader {

  /**
   * Get a promise that is resolved when all the named packages have been loaded.
   *
   * @param {string[]} names  The packages to wait for
   * @returns {Promise}       A promise that resolves when all the named packages are ready
   */
  export function ready(...names: string[]): Promise<string[]> {
    if (names.length === 0) {
      names = Array.from(Package.packages.keys());
    }
    const promises = [];
    for (const name of names) {
      const extension = Package.packages.get(name) || new Package(name, true);
      promises.push(extension.promise);
    }
    return Promise.all(promises);
  }

  /**
   * Load the named packages and return a promise that is resolved when they are all loaded
   *
   * @param {string[]} names  The packages to load
   * @returns {Promise}       A promise that resolves when all the named packages are ready
   */
  export function load(...names: string[]): Promise<void | string[]> {
    if (names.length === 0) {
      return Promise.resolve();
    }
    const promises = [];
    for (const name of names) {
      let extension = Package.packages.get(name);
      if (!extension) {
        extension = new Package(name);
        extension.provides(CONFIG.provides[name]);
      }
      extension.checkNoLoad();
      promises.push(extension.promise);
    }
    Package.loadAll();
    return Promise.all(promises);
  }

  /**
   * Indicate that the named packages are being loaded by hand (e.g., as part of a larger package).
   *
   * @param {string[]} names  The packages to load
   */
  export function preLoad(...names: string[]) {
    for (const name of names) {
      let extension = Package.packages.get(name);
      if (!extension) {
        extension = new Package(name, true);
        extension.provides(CONFIG.provides[name]);
      }
      extension.loaded();
    }
  }

  /**
   * The default function to perform when all the packages are loaded
   */
  export function defaultReady() {
    if (typeof MathJax.startup !== 'undefined') {
      MathJax.config.startup.ready();
    }
  }

  /**
   * Get the root location for where the MathJax package files are found
   *
   * @returns {string}   The root location (directory for node.js, URL for browser)
   */
  export function getRoot(): string {
    let root = __dirname + '/../../es5';
    if (typeof document !== 'undefined') {
      const script = document.currentScript || document.getElementById('MathJax-script');
      if (script) {
        root = (script as HTMLScriptElement).src.replace(/\/[^\/]*$/, '');
      }
    }
    return root;
  }

}

/**
 * Export the global MathJax object for convenience
 */
export const MathJax = MJGlobal as MathJaxObject;

/*
 * If the loader hasn't been added to the MathJax variable,
 *   Add the loader configuration, library, and data objects.
 */
if (typeof MathJax.loader === 'undefined') {

  combineDefaults(MathJax.config, 'loader', {
    paths: {
      mathjax: Loader.getRoot()
    },
    source: {},
    dependencies: {},
    provides: {},
    load: [],
    ready: Loader.defaultReady.bind(Loader),
    failed: (error: PackageError) => console.log(`MathJax(${error.package || '?'}): ${error.message}`),
    require: null
  });
  combineWithMathJax({
    loader: Loader
  });

}

/**
 * Export the loader configuration for convenience
 */
export const CONFIG = MathJax.config.loader;

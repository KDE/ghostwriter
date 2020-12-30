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
 * @fileoverview  Loads SRE (if necessary) and provides a promise for
 *                when it is ready.
 *
 * @author dpvc@mathjax.org (Davide Cervone)
 */

import {asyncLoad} from '../util/AsyncLoad.js';

/**
 * The name of the modiule to load, if necessary
 */
const SRELIB = (typeof window === 'undefined' ? './a11y/sre-node.js' :
                '../../../speech-rule-engine/lib/sre_browser.js');

/**
 * The promise for loading the SRE library, if it is not already loaded
 */
const srePromise = (typeof sre === 'undefined' ? asyncLoad(SRELIB) : Promise.resolve());

/**
 * Values to control the polling for when SRE is ready
 */
const SRE_DELAY = 100;         // in milliseconds
const SRE_TIMEOUT = 20 * 1000; // 20 seconds

/**
 * A promise that resolves when SRE is loaded and ready, and rejects if
 * SRE can't be loaded, or does not become ready within the timout period.
 */
export const sreReady = function() {
  return new Promise<void>((resolve, reject) => {
    srePromise.then(() => {
      const start = new Date().getTime();
      const checkSRE = function () {
        if (sre.Engine.isReady()) {
          resolve();
        } else {
          if (new Date().getTime() - start < SRE_TIMEOUT) {
            setTimeout(checkSRE, SRE_DELAY);
          } else {
            reject('Timed out waiting for Speech-Rule-Engine');
          }
        }
      };
      checkSRE();
    }).catch((error: Error) => reject(error.message || error));
  });
};

/*************************************************************
 *
 *  Copyright (c) 2019 The MathJax Consortium
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
 * @fileoverview    Configuration file for the configmacros package.
 *
 * @author dpvc@mathjax.org (Davide P. Cervone)
 */

import {Configuration, ParserConfiguration} from '../Configuration.js';
import {expandable} from '../../../util/Options.js';
import {CommandMap} from '../SymbolMap.js';
import {Macro} from '../Symbol.js';
import NewcommandMethods from '../newcommand/NewcommandMethods.js';
import {TeX} from '../../tex.js';

/**
 * The name to use for the macros map
 */
const MACROSMAP = 'configmacros-map';

/**
 * Create the command map for the macros
 *
 * @param {Configuration} config   The configuration object for the input jax
 */
function configmacrosInit(config: ParserConfiguration) {
  new CommandMap(MACROSMAP, {}, {});
  config.append(Configuration.local({handler: {macro: [MACROSMAP]}, priority: 3}));
}

/**
 * Create the user-defined macros from the macros option
 *
 * @param {Configuration} config   The configuration object for the input jax
 * @param {TeX} jax                The TeX input jax
 */
function configmacrosConfig(_config: ParserConfiguration, jax: TeX<any, any, any>) {
  const macrosMap = jax.parseOptions.handlers.retrieve(MACROSMAP) as CommandMap;
  const macros = jax.parseOptions.options.macros;
  for (const cs of Object.keys(macros)) {
    const def = (typeof macros[cs] === 'string' ? [macros[cs]] : macros[cs]);
    const macro = Array.isArray(def[2]) ?
      new Macro(cs, NewcommandMethods.MacroWithTemplate, def.slice(0, 2).concat(def[2])) :
      new Macro(cs, NewcommandMethods.Macro, def);
    macrosMap.add(cs, macro);
  }
}

/**
 * The configuration object for configmacros
 */
export const ConfigMacrosConfiguration = Configuration.create(
  'configmacros', {
    init: configmacrosInit,
    config: configmacrosConfig,
    options: {macros: expandable({})}
  }
);

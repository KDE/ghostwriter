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
 * @fileoverview Configuration file for the mhchem package.
 *
 * @author v.sorge@mathjax.org (Volker Sorge)
 */

import {Configuration} from '../Configuration.js';
import {CommandMap} from '../SymbolMap.js';
import {ParseMethod} from '../Types.js';
import TexError from '../TexError.js';
import TexParser from '../TexParser.js';
import BaseMethods from '../base/BaseMethods.js';
import {AmsMethods} from '../ams/AmsMethods.js';
import {mhchemParser, texify} from './mhchem_parser.js';

// Namespace
let MhchemMethods: Record<string, ParseMethod> = {};

MhchemMethods.Macro = BaseMethods.Macro;
MhchemMethods.xArrow = AmsMethods.xArrow;

/**
 * @param{TeXParser} parser   The parser for this expression
 * @param{string} name        The macro name being called
 * @param{string} machine     The name of the fininte-state machine to use
 */
MhchemMethods.Machine = function(parser: TexParser, name: string, machine: string) {
  try {
    let arg = parser.GetArgument(name);
    let data = mhchemParser.go(arg, machine);
    // TODO: Harvest chemical information here from data, test looping through
    //       the array.
    let tex = texify.go(data);
    parser.string = tex + parser.string.substr(parser.i);
    parser.i = 0;
  } catch (err) {
    throw new TexError(err[0], err[1], err.slice(2));
  }
};

new CommandMap(
  'mhchem', {
    ce: ['Machine', 'ce'],
    pu: ['Machine', 'pu'],
    longrightleftharpoons: ['Macro',
                            '\\stackrel{\\textstyle{-}\\!\\!{\\rightharpoonup}}{\\smash{{\\leftharpoondown}\\!\\!{-}}}'],
    longRightleftharpoons: ['Macro',
                            '\\stackrel{\\textstyle{-}\\!\\!{\\rightharpoonup}}{\\smash{\\leftharpoondown}}'],
    longLeftrightharpoons: ['Macro',
                            '\\stackrel{\\textstyle\\vphantom{{-}}{\\rightharpoonup}}{\\smash{{\\leftharpoondown}\\!\\!{-}}}'],
    longleftrightarrows: ['Macro',
                          '\\stackrel{\\longrightarrow}{\\smash{\\longleftarrow}\\Rule{0px}{.25em}{0px}}'],
    //
    //  Needed for \bond for the ~ forms
    //  Not perfectly aligned when zoomed in, but on 100%
    //
    tripledash: ['Macro',
                 '\\vphantom{-}\\raise2mu{\\kern2mu\\tiny\\text{-}\\kern1mu\\text{-}\\kern1mu\\text{-}\\kern2mu}'],
    xrightarrow: ['xArrow', 0x2192, 5, 6],
    xleftarrow:  ['xArrow', 0x2190, 7, 3],
    xleftrightarrow:    ['xArrow', 0x2194, 6, 6],
    xrightleftharpoons: ['xArrow', 0x21CC, 5, 7],   // FIXME:  doesn't stretch in HTML-CSS output
    xRightleftharpoons: ['xArrow', 0x21CC, 5, 7],   // FIXME:  how should this be handled?
    xLeftrightharpoons: ['xArrow', 0x21CC, 5, 7]
  },
  MhchemMethods
);


export const MhchemConfiguration = Configuration.create(
  'mhchem', {handler: {macro: ['mhchem']}}
);

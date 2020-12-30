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
 * @fileoverview  Loads SRE for node and creates the global sre variable
 *                with sre.Engine.isReady(), like in the browser version
 *
 * @author dpvc@mathjax.org (Davide Cervone)
 */

import * as SRE from 'speech-rule-engine';

declare const global: any;

/**
 * The global sre with sre.Engine.isReady() and sre.toEnriched()
 */
global.SRE = SRE;
global.sre = Object.create(SRE);
global.sre.Engine = {
  /**
   * @return {boolean}   True when SRE is ready
   */
  isReady(): boolean {
    return SRE.engineReady();
  }
};

export {};

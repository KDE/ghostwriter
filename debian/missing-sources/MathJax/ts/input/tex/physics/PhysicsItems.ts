/*************************************************************
 *
 *  Copyright (c) 2009-2018 The MathJax Consortium
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
 * @fileoverview Stack items for the physics package.
 *
 * @author v.sorge@mathjax.org (Volker Sorge)
 */


import {CheckType, BaseItem, StackItem} from '../StackItem.js';
import ParseUtil from '../ParseUtil.js';
import TexParser from '../TexParser.js';


export class AutoOpen extends BaseItem {

  /**
   * @override
   */
  public get kind() {
    return 'auto open';
  }


  /**
   * @override
   */
  get isOpen() {
    return true;
  }


  /**
   * @override
   */
  public toMml() {
    // Smash and right/left
    let parser = this.factory.configuration.parser;
    let right = this.getProperty('right') as string;
    if (this.getProperty('smash')) {
      let mml = super.toMml();
      const smash = parser.create('node', 'mpadded', [mml],
                                  {height: 0, depth: 0});
      this.Clear();
      this.Push(parser.create('node', 'TeXAtom', [smash]));
    }
    if (right) {
      this.Push(new TexParser(right, parser.stack.env,
                              parser.configuration).mml());
    }
    let mml = super.toMml();
    return ParseUtil.fenced(this.factory.configuration,
                            this.getProperty('open') as string,
                            mml,
                            this.getProperty('close') as string,
                            this.getProperty('big') as string);
  }

  /**
   * @override
   */
  public checkItem(item: StackItem): CheckType {
    let close = item.getProperty('autoclose');
    if (close && close === this.getProperty('close')) {
      if (this.getProperty('ignore')) {
        this.Clear();
        return [[], true];
      }
      return [[this.toMml()], true];
    }
    return super.checkItem(item);
  }

}

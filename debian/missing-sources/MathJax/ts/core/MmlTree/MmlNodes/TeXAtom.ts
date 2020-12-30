/*************************************************************
 *
 *  Copyright (c) 2017 The MathJax Consortium
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
 * @fileoverview  Implements the TeXAtom node
 *
 * @author dpvc@mathjax.org (Davide Cervone)
 */

import {PropertyList} from '../../Tree/Node.js';
import {AbstractMmlBaseNode, MmlNode, TEXCLASS} from '../MmlNode.js';
import {MmlMo} from './mo.js';

/*****************************************************************/
/**
 *  Implements the TeXAtom node class (subclass of AbstractMmlBaseNode)
 */

export class TeXAtom extends AbstractMmlBaseNode {

  /**
   * @override
   */
  public static defaults: PropertyList = {
    ...AbstractMmlBaseNode.defaults
  };

  /**
   * TeX class is ORD
   */
  public texClass = TEXCLASS.ORD;

  /**
   * @override
   */
  public get kind() {
    return 'TeXAtom';
  }

  /**
   * Inferred mrow with any number of children
   * @override
   */
  public get arity() {
    return -1;
  }

  /**
   * This element is not considered a MathML container
   * @override
   */
  public get notParent() {
    return true;
  }

  /**
   * @override
   */
  public setTeXclass(prev: MmlNode) {
    this.childNodes[0].setTeXclass(null);
    return this.adjustTeXclass(prev);
  }

  /**
   * (Replaced below by the version from the MmlMo node)
   *
   * @override
   */
  public adjustTeXclass(prev: MmlNode) {
    return prev;
  }

}
/**
 *  Use the method from the MmlMo class
 */
TeXAtom.prototype.adjustTeXclass = MmlMo.prototype.adjustTeXclass;

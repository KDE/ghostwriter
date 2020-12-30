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
 * @fileoverview  Implements the SVGmmultiscripts wrapper for the MmlMmultiscripts object
 *
 * @author dpvc@mathjax.org (Davide Cervone)
 */

import {SVGWrapper, Constructor} from '../Wrapper.js';
import {SVGmsubsup} from './msubsup.js';
import {CommonMmultiscriptsMixin} from '../../common/Wrappers/mmultiscripts.js';
import {MmlMmultiscripts} from '../../../core/MmlTree/MmlNodes/mmultiscripts.js';

/*****************************************************************/
/**
 * The SVGmmultiscripts wrapper for the MmlMmultiscripts object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class SVGmmultiscripts<N, T, D> extends
CommonMmultiscriptsMixin<SVGWrapper<any, any, any>, Constructor<SVGmsubsup<any, any, any>>>(SVGmsubsup) {

  /**
   * The mmultiscripts wrapper
   */
  public static kind = MmlMmultiscripts.prototype.kind;

  /**
   * @override
   */
  public toSVG(parent: N) {
    const svg = this.standardSVGnode(parent);
    const data = this.getScriptData();
    //
    //  Combine the bounding boxes of the pre- and post-scripts,
    //  and get the resulting baseline offsets
    //
    const sub = this.combinePrePost(data.sub, data.psub);
    const sup = this.combinePrePost(data.sup, data.psup);
    const [u, v] = this.getUVQ(data.base, sub, sup);
    //
    //  Place the pre-scripts, then the base, then the post-scripts
    //
    let x = 0;  // scriptspace
    if (data.numPrescripts) {
      x = this.addScripts(.05, u, v, true, this.firstPrescript, data.numPrescripts);
    }
    const base = this.baseChild;
    base.toSVG(svg);
    base.place(x, 0);
    x += base.getBBox().w;
    if (data.numScripts) {
      this.addScripts(x, u, v, false, 1, data.numScripts);
    }
  }

  /**
   * Create a table with the super and subscripts properly separated and aligned.
   *
   * @param {number} x       The x offset of the scripts
   * @param {number} u       The baseline offset for the superscripts
   * @param {number} v       The baseline offset for the subscripts
   * @param {boolean} isPre  True for prescripts, false for scripts
   * @param {number} i       The starting index for the scripts
   * @param {number} n       The number of sub/super-scripts
   * @return {number}        The right-hand offset of the scripts
   */
  protected addScripts(x: number, u: number, v: number, isPre: boolean, i: number, n: number): number {
    const adaptor = this.adaptor;
    const supRow = adaptor.append(this.element, this.svg('g'));
    const subRow = adaptor.append(this.element, this.svg('g'));
    this.place(x, u, supRow);
    this.place(x, v, subRow);
    let m = i + 2 * n;
    let dx = 0;
    while (i < m) {
      const [sub, sup] = [this.childNodes[i++], this.childNodes[i++]];
      const [subbox, supbox] = [sub.getBBox(), sup.getBBox()];
      const [subr, supr] = [subbox.rscale, supbox.rscale];
      const w = Math.max(subbox.w * subr, supbox.w * supr);
      sub.toSVG(subRow);
      sup.toSVG(supRow);
      sub.place(dx + (isPre ? w - subbox.w * subr : 0), 0);
      sup.place(dx + (isPre ? w - supbox.w * supr : 0), 0);
      dx += w;
    }
    return x + dx;
  }

}

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
 * @fileoverview  Implements the a base mixin for CommonMsubsup, CommonMunderover
 *                and their relatives.  (Since munderover can become msubsup
 *                when movablelimits is set, munderover needs to be able to
 *                do the same thing as msubsup in some cases.)
 *
 * @author dpvc@mathjax.org (Davide Cervone)
 */

import {AnyWrapper, WrapperConstructor, Constructor, AnyWrapperClass} from '../Wrapper.js';
import {CommonMo} from './mo.js';
import {MmlMsubsup} from '../../../core/MmlTree/MmlNodes/msubsup.js';
import {BBox} from '../../../util/BBox.js';
import {DIRECTION} from '../FontData.js';

/*
 * Mutliply italic correction by this much (improve horizontal shift for italic characters)
 */
const DELTA = 1.5;

/*****************************************************************/
/**
 * The CommonScriptbase interface
 *
 * @template W  The child-node Wrapper class
 */
export interface CommonScriptbase<W extends AnyWrapper> extends AnyWrapper {

  /**
   * The core mi or mo of the base (or the base itself if there isn't one)
   */
  baseCore: W;

  /**
   * The base element's wrapper
   */
  readonly baseChild: W;

  /**
   * The script element's wrapper (overridden in subclasses)
   */
  readonly script: W;

  /**
   * @return {boolean}  True if the base is an mi, mn, or mo (not a largeop) consisting of a single character
   */
  isCharBase(): boolean;

  /***************************************************************************/
  /*
   *  Methods for sub-sup nodes
   */

  /**
   * @return {number}  The ic for the core element
   */
  coreIC(): number;

  /**
   * @return {number}   The relative scaling of the base
   */
  coreScale(): number;

  /**
   * Get the shift for the script (implemented in subclasses)
   *
   * @param {BBox} bbox   The bounding box of the base element
   * @param {BBox} sbox   The bounding box of the script element
   * @return {number[]}   The horizontal and vertical offsets for the script
   */
  getOffset(bbox: BBox, sbox: BBox): number[];

  /**
   * Get the shift for a subscript (TeXBook Appendix G 18ab)
   *
   * @param {BBox} bbox   The bounding box of the base element
   * @param {BBox} sbox   The bounding box of the superscript element
   * @return {number}     The vertical offset for the script
   */
  getV(bbox: BBox, sbox: BBox): number;

  /**
   * Get the shift for a superscript (TeXBook Appendix G 18acd)
   *
   * @param {BBox} bbox   The bounding box of the base element
   * @param {BBox} sbox   The bounding box of the superscript element
   * @return {number}     The vertical offset for the script
   */
  getU(bbox: BBox, sbox: BBox): number;

  /***************************************************************************/
  /*
   *  Methods for under-over nodes
   */

  /**
   * @return {boolean}  True if the base has movablelimits (needed by munderover)
   */
  hasMovableLimits(): boolean;

  /**
   * Get the separation and offset for overscripts (TeXBoox Appendix G 13, 13a)
   *
   * @param {BBox} basebox  The bounding box of the base
   * @param {BBox} overbox  The bounding box of the overscript
   * @return {number[]}     The separation between their boxes, and the offset of the overscript
   */
  getOverKU(basebox: BBox, overbox: BBox): number[];

  /**
   * Get the separation and offset for underscripts (TeXBoox Appendix G 13, 13a)
   *
   * @param {BBox} basebox   The bounding box of the base
   * @param {BBox} underbox  The bounding box of the underscript
   * @return {number[]}      The separation between their boxes, and the offset of the underscript
   */
  getUnderKV(basebox: BBox, underbox: BBox): number[];

  /**
   * @param {BBox[]} boxes     The bounding boxes whose offsets are to be computed
   * @param {number[]=} delta  The initial x offsets of the boxes
   * @return {number[]}        The actual offsets needed to center the boxes in the stack
   */
  getDeltaW(boxes: BBox[], delta?: number[]): number[];

  /**
   * @param {boolean=} noskew   Whether to ignore the skew amount
   * @return {number}           The offset for under and over
   */
  getDelta(noskew?: boolean): number;

  /**
   * Handle horizontal stretching of children to match greatest width
   *  of all children
   */
  stretchChildren(): void;

}

export interface CommonScriptbaseClass extends AnyWrapperClass {
  /**
   * Set to true for munderover/munder/mover/msup (Appendix G 13)
   */
  useIC: boolean;
}

/**
 * Shorthand for the CommonScriptbase constructor
 *
 * @template W  The child-node Wrapper class
 */
export type ScriptbaseConstructor<W extends AnyWrapper> = Constructor<CommonScriptbase<W>>;

/*****************************************************************/
/**
 * A base class for msup/msub/msubsup and munder/mover/munderover
 * wrapper mixin implementations
 *
 * @template W  The child-node Wrapper class
 * @template T  The Wrapper class constructor type
 */
export function CommonScriptbaseMixin<
  W extends AnyWrapper,
  T extends WrapperConstructor
>(Base: T): ScriptbaseConstructor<W> & T {

  return class extends Base {

    /**
     * Set to true for munderover/munder/mover/msup (Appendix G 13)
     */
    public static useIC: boolean = false;

    /**
     * The core mi or mo of the base (or the base itself if there isn't one)
     */
    public baseCore: W;

    /**
     * @return {W}  The base element's wrapper
     */
    public get baseChild(): W {
      return this.childNodes[(this.node as MmlMsubsup).base];
    }

    /**
     * @return {W}  The script element's wrapper (overridden in subclasses)
     */
    public get script(): W {
      return this.childNodes[1];
    }

    /**
     * @override
     */
    constructor(...args: any[]) {
      super(...args);
      //
      //  Find the base core
      //
      let core = this.baseCore = this.childNodes[0];
      if (!core) return;
      while (core.childNodes.length === 1 &&
             (core.node.isKind('mrow') || core.node.isKind('TeXAtom') ||
              core.node.isKind('mstyle') || core.node.isKind('mpadded') ||
              core.node.isKind('mphantom') || core.node.isKind('semantics'))) {
        core = core.childNodes[0];
        if (!core) return;
      }
      if (!('noIC' in core)) return;
      this.baseCore = core;
      //
      //  Check if the base is a mi or mo that needs italic correction removed
      //
      if (!(this.constructor as CommonScriptbaseClass).useIC) {
        (core as CommonMo).noIC = true;
      }
    }

    /**
     * This gives the common bbox for msub and msup.  It is overridden
     * for all the others (msubsup, munder, mover, munderover).
     *
     * @override
     */
    public computeBBox(bbox: BBox, recompute: boolean = false) {
      const basebox = this.baseChild.getBBox();
      const scriptbox = this.script.getBBox();
      const [x, y] = this.getOffset(basebox, scriptbox);
      bbox.append(basebox);
      bbox.combine(scriptbox, bbox.w + x, y);
      bbox.w += this.font.params.scriptspace;
      bbox.clean();
      this.setChildPWidths(recompute);
    }

    /**
     * @return {number}  The ic for the core element
     */
    public coreIC(): number {
      const corebox = this.baseCore.getBBox();
      return (corebox.ic ? 1.05 * corebox.ic + .05 : 0);
    }

    /**
     * @return {number}   The relative scaling of the base
     */
    public coreScale(): number {
      let scale = this.baseChild.getBBox().rscale;
      let base = this.baseChild;
      while ((base.node.isKind('mstyle') || base.node.isKind('mrow') || base.node.isKind('TeXAtom'))
             && base.childNodes.length === 1) {
        base = base.childNodes[0];
        scale *= base.getBBox().rscale;
      }
      return scale;
    }

    /**
     * @return {boolean}  True if the base is an mi, mn, or mo (not a largeop) consisting of a single character
     */
    public isCharBase(): boolean {
      let base = this.baseChild;
      while ((base.node.isKind('mstyle') || base.node.isKind('mrow')) && base.childNodes.length === 1) {
        base = base.childNodes[0];
      }
      return ((base.node.isKind('mo') || base.node.isKind('mi') || base.node.isKind('mn')) &&
              base.bbox.rscale === 1 && Array.from(base.getText()).length === 1 &&
              !base.node.attributes.get('largeop'));
    }

    /***************************************************************************/
    /*
     *  Methods for sub-sup nodes
     */

    /**
     * Get the shift for the script (implemented in subclasses)
     *
     * @param {BBox} bbox   The bounding box of the base element
     * @param {BBox} sbox   The bounding box of the script element
     * @return {[number, number]}   The horizontal and vertical offsets for the script
     */
    public getOffset(_bbox: BBox, _sbox: BBox): [number, number] {
      return [0, 0];
    }

    /**
     * Get the shift for a subscript (TeXBook Appendix G 18ab)
     *
     * @param {BBox} bbox   The bounding box of the base element
     * @param {BBox} sbox   The bounding box of the superscript element
     * @return {number}     The vertical offset for the script
     */
    public getV(bbox: BBox, sbox: BBox): number {
      const tex = this.font.params;
      const subscriptshift = this.length2em(this.node.attributes.get('subscriptshift'), tex.sub1);
      return Math.max(
        this.isCharBase() ? 0 : bbox.d * bbox.rscale + tex.sub_drop * sbox.rscale,
        subscriptshift,
        sbox.h * sbox.rscale - (4 / 5) * tex.x_height
      );
    }

    /**
     * Get the shift for a superscript (TeXBook Appendix G 18acd)
     *
     * @param {BBox} bbox   The bounding box of the base element
     * @param {BBox} sbox   The bounding box of the superscript element
     * @return {number}     The vertical offset for the script
     */
    public getU(bbox: BBox, sbox: BBox): number {
      const tex = this.font.params;
      const attr = this.node.attributes.getList('displaystyle', 'superscriptshift');
      const prime = this.node.getProperty('texprimestyle');
      const p = prime ? tex.sup3 : (attr.displaystyle ? tex.sup1 : tex.sup2);
      const superscriptshift = this.length2em(attr.superscriptshift, p);
      return Math.max(
        this.isCharBase() ? 0 : bbox.h * bbox.rscale - tex.sup_drop * sbox.rscale,
        superscriptshift,
        sbox.d * sbox.rscale + (1 / 4) * tex.x_height
      );
    }

    /***************************************************************************/
    /*
     *  Methods for under-over nodes
     */

    /**
     * @return {boolean}  True if the base has movablelimits (needed by munderover)
     */
    public hasMovableLimits(): boolean {
      const display = this.node.attributes.get('displaystyle');
      const mo = this.baseChild.coreMO().node;
      return (!display && !!mo.attributes.get('movablelimits'));
    }

    /**
     * Get the separation and offset for overscripts (TeXBoox Appendix G 13, 13a)
     *
     * @param {BBox} basebox  The bounding box of the base
     * @param {BBox} overbox  The bounding box of the overscript
     * @return {[number, number]}     The separation between their boxes, and the offset of the overscript
     */
    public getOverKU(basebox: BBox, overbox: BBox): [number, number] {
      const accent = this.node.attributes.get('accent') as boolean;
      const tex = this.font.params;
      const d = overbox.d * overbox.rscale;
      const k = (accent ? tex.rule_thickness :
                 Math.max(tex.big_op_spacing1, tex.big_op_spacing3 - Math.max(0, d))) -
        (this.baseChild.node.isKind('munderover') ? .1 : 0);
      return [k, basebox.h * basebox.rscale + k + d];
    }

    /**
     * Get the separation and offset for underscripts (TeXBoox Appendix G 13, 13a)
     *
     * @param {BBox} basebox   The bounding box of the base
     * @param {BBox} underbox  The bounding box of the underscript
     * @return {[number, number]}      The separation between their boxes, and the offset of the underscript
     */
    public getUnderKV(basebox: BBox, underbox: BBox): [number, number] {
      const accent = this.node.attributes.get('accentunder') as boolean;
      const tex = this.font.params;
      const h = underbox.h * underbox.rscale;
      const k = (accent ? tex.rule_thickness :
                 Math.max(tex.big_op_spacing2, tex.big_op_spacing4 - h)) -
        (this.baseChild.node.isKind('munderover') ? .1 : 0);
      return [k, -(basebox.d * basebox.rscale + k + h)];
    }

    /**
     * @param {BBox[]} boxes     The bounding boxes whose offsets are to be computed
     * @param {number[]=} delta  The initial x offsets of the boxes
     * @return {number[]}        The actual offsets needed to center the boxes in the stack
     */
    public getDeltaW(boxes: BBox[], delta: number[] = [0, 0, 0]): number[] {
      const align = this.node.attributes.get('align');
      const widths = boxes.map(box => box.w * box.rscale);
      const w = Math.max(...widths);
      const dw = [];
      let m = 0;
      for (const i of widths.keys()) {
        dw[i] = (align === 'center' ? (w - widths[i]) / 2 :
                 align === 'right' ? w - widths[i] : 0) + delta[i];
        if (dw[i] < m) {
          m = -dw[i];
        }
      }
      if (m) {
        for (const i of dw.keys()) {
          dw[i] += m;
        }
      }
      return dw;
    }

    /**
     * @param {boolean=} noskew   Whether to ignore the skew amount
     * @return {number}           The offset for under and over
     */
    public getDelta(noskew: boolean = false): number {
      const accent = this.node.attributes.get('accent');
      const ddelta = (accent && !noskew ? this.baseChild.coreMO().bbox.sk : 0);
      return (DELTA * this.baseCore.bbox.ic / 2 + ddelta) * this.coreScale();
    }

    /**
     * Handle horizontal stretching of children to match greatest width
     *  of all children
     */
    public stretchChildren() {
      let stretchy: AnyWrapper[] = [];
      //
      //  Locate and count the stretchy children
      //
      for (const child of this.childNodes) {
        if (child.canStretch(DIRECTION.Horizontal)) {
          stretchy.push(child);
        }
      }
      let count = stretchy.length;
      let nodeCount = this.childNodes.length;
      if (count && nodeCount > 1) {
        let W = 0;
        //
        //  If all the children are stretchy, find the largest one,
        //  otherwise, find the width of the non-stretchy children.
        //
        let all = (count > 1 && count === nodeCount);
        for (const child of this.childNodes) {
          const noStretch = (child.stretch.dir === DIRECTION.None);
          if (all || noStretch) {
            const {w, rscale} = child.getBBox(noStretch);
            if (w * rscale > W) W = w * rscale;
          }
        }
        //
        //  Stretch the stretchable children
        //
        for (const child of stretchy) {
          (child.coreMO() as CommonMo).getStretchedVariant([W / child.bbox.rscale]);
        }
      }
    }

  };

}

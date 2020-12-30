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
 * @fileoverview  Implements the CommonMo wrapper mixin for the MmlMo object
 *
 * @author dpvc@mathjax.org (Davide Cervone)
 */

import {AnyWrapper, WrapperConstructor, Constructor} from '../Wrapper.js';
import {MmlMo} from '../../../core/MmlTree/MmlNodes/mo.js';
import {BBox} from '../../../util/BBox.js';
import {DelimiterData} from '../FontData.js';
import {DIRECTION, NOSTRETCH} from '../FontData.js';

/*****************************************************************/
/**
 * Convert direction to letter
 */
export const DirectionVH: {[n: number]: string} = {
  [DIRECTION.Vertical]: 'v',
  [DIRECTION.Horizontal]: 'h'
};

/*****************************************************************/
/**
 * The CommonMo interface
 */
export interface CommonMo extends AnyWrapper {
  /**
   * True if no italic correction should be used
   */
  noIC: boolean;

  /**
   * The font size that a stretched operator uses.
   * If -1, then stretch arbitrarily, and bbox gives the actual height, depth, width
   */
  size: number;

  /**
   * True if used as an accent in an munderover construct
   */
  isAccent: boolean;

  /**
   * Determint variant for vertically/horizontally stretched character
   *
   * @param {number[]} WH  size to stretch to, either [W] or [H, D]
   * @param {boolean} exact  True if not allowed to use delimiter factor and shortfall
   */
  getStretchedVariant(WH: number[], exact?: boolean): void;

  /**
   * @param {string} name   The name of the attribute to get
   * @param {number} value  The default value to use
   * @return {number}       The size in em's of the attribute (or the default value)
   */
  getSize(name: string, value: number): number;

  /**
   * @param {number[]} WH  Either [W] for width, [H, D] for height and depth, or [] for min/max size
   * @return {number}      Either the width or the total height of the character
   */
  getWH(WH: number[]): number;

  /**
   * @param {number[]} WHD     The [W] or [H, D] being requested from the parent mrow
   * @param {number} D         The full dimension (including symmetry, etc)
   * @param {DelimiterData} C  The delimiter data for the stretchy character
   */
  getStretchBBox(WHD: number[], D: number, C: DelimiterData): void;

  /**
   * @param {number[]} WHD     The [H, D] being requested from the parent mrow
   * @param {number} HD        The full height (including symmetry, etc)
   * @param {DelimiterData} C  The delimiter data for the stretchy character
   * @return {number[]}        The height and depth for the vertically stretched delimiter
   */
  getBaseline(WHD: number[], HD: number, C: DelimiterData): number[];
}

/**
 * Shorthand for the CommonMo constructor
 */
export type MoConstructor = Constructor<CommonMo>;

/*****************************************************************/
/**
 * The CommomMo wrapper mixin for the MmlMo object
 *
 * @template T  The Wrapper class constructor type
 */
export function CommonMoMixin<T extends WrapperConstructor>(Base: T): MoConstructor & T {

  return class extends Base {

    /**
     * True if no italic correction should be used
     */
    public noIC: boolean = false;

    /**
     * The font size that a stretched operator uses.
     * If -1, then stretch arbitrarily, and bbox gives the actual height, depth, width
     */
    public size: number = null;

    /**
     * True if used as an accent in an munderover construct
     */
    public isAccent: boolean;

    /**
     * @override
     */
    constructor(...args: any[]) {
      super(...args);
      this.isAccent = (this.node as MmlMo).isAccent;
    }

    /**
     * @override
     */
    public computeBBox(bbox: BBox, _recompute: boolean = false) {
      const stretchy = (this.stretch.dir !== DIRECTION.None);
      if (stretchy && this.size === null) {
        this.getStretchedVariant([0]);
      }
      if (stretchy && this.size < 0) return;
      super.computeBBox(bbox);
      this.copySkewIC(bbox);
      if (this.noIC) {
        bbox.w -= bbox.ic;
      }
      if (this.node.attributes.get('symmetric') &&
          this.stretch.dir !== DIRECTION.Horizontal) {
        const d = ((bbox.h + bbox.d) / 2 + this.font.params.axis_height) - bbox.h;
        bbox.h += d;
        bbox.d -= d;
      }
    }

    /**
     * @override
     */
    public getVariant() {
      if (this.node.attributes.get('largeop')) {
        this.variant = (this.node.attributes.get('displaystyle') ? '-largeop' : '-smallop');
      } else {
        super.getVariant();
      }
    }

    /**
     * @override
     */
    public canStretch(direction: DIRECTION) {
      if (this.stretch.dir !== DIRECTION.None) {
        return this.stretch.dir === direction;
      }
      const attributes = this.node.attributes;
      if (!attributes.get('stretchy')) return false;
      const c = this.getText();
      if (Array.from(c).length !== 1) return false;
      const delim = this.font.getDelimiter(c.codePointAt(0));
      this.stretch = (delim && delim.dir === direction ? delim : NOSTRETCH);
      return this.stretch.dir !== DIRECTION.None;
    }

    /**
     * Determint variant for vertically/horizontally stretched character
     *
     * @param {number[]} WH  size to stretch to, either [W] or [H, D]
     * @param {boolean} exact  True if not allowed to use delimiter factor and shortfall
     */
    public getStretchedVariant(WH: number[], exact: boolean = false) {
      if (this.stretch.dir !== DIRECTION.None) {
        let D = this.getWH(WH);
        const min = this.getSize('minsize', 0);
        const max = this.getSize('maxsize', Infinity);
        //
        //  Clamp the dimension to the max and min
        //  then get the minimum size via TeX rules
        //
        D = Math.max(min, Math.min(max, D));
        const m = (min || exact ? D : Math.max(D * this.font.params.delimiterfactor / 1000,
                                               D - this.font.params.delimitershortfall));
        //
        //  Look through the delimiter sizes for one that matches
        //
        const delim = this.stretch;
        const c = delim.c || this.getText().codePointAt(0);
        let i = 0;
        if (delim.sizes) {
          for (const d of delim.sizes) {
            if (d >= m) {
              this.variant = this.font.getSizeVariant(c, i);
              this.size = i;
              return;
            }
            i++;
          }
        }
        //
        //  No size matches, so if we can make multi-character delimiters,
        //  record the data for that, otherwise, use the largest fixed size.
        //
        if (delim.stretch) {
          this.size = -1;
          this.invalidateBBox();
          this.getStretchBBox(WH, D, delim);
        } else {
          this.variant = this.font.getSizeVariant(c, i - 1);
          this.size = i - 1;
        }
      }
    }

    /**
     * @param {string} name   The name of the attribute to get
     * @param {number} value  The default value to use
     * @return {number}       The size in em's of the attribute (or the default value)
     */
    public getSize(name: string, value: number): number {
      let attributes = this.node.attributes;
      if (attributes.isSet(name)) {
        value = this.length2em(attributes.get(name), 1, 1); // FIXME: should use height of actual character
      }
      return value;
    }

    /**
     * @param {number[]} WH  Either [W] for width, [H, D] for height and depth, or [] for min/max size
     * @return {number}      Either the width or the total height of the character
     */
    public getWH(WH: number[]): number {
      if (WH.length === 0) return 0;
      if (WH.length === 1) return WH[0];
      let [H, D] = WH;
      const a = this.font.params.axis_height;
      return (this.node.attributes.get('symmetric') ? 2 * Math.max(H - a, D + a) : H + D);
    }

    /**
     * @param {number[]} WHD     The [W] or [H, D] being requested from the parent mrow
     * @param {number} D         The full dimension (including symmetry, etc)
     * @param {DelimiterData} C  The delimiter data for the stretchy character
     */
    public getStretchBBox(WHD: number[], D: number, C: DelimiterData) {
      if (C.hasOwnProperty('min') && C.min > D) {
        D = C.min;
      }
      let [h, d, w] = C.HDW;
      if (this.stretch.dir === DIRECTION.Vertical) {
        [h, d] = this.getBaseline(WHD, D, C);
      } else {
        w = D;
      }
      this.bbox.h = h;
      this.bbox.d = d;
      this.bbox.w = w;
    }

    /**
     * @param {number[]} WHD     The [H, D] being requested from the parent mrow
     * @param {number} HD        The full height (including symmetry, etc)
     * @param {DelimiterData} C  The delimiter data for the stretchy character
     * @return {[number, number]}        The height and depth for the vertically stretched delimiter
     */
    public getBaseline(WHD: number[], HD: number, C: DelimiterData): [number, number] {
      const hasWHD = (WHD.length === 2 && WHD[0] + WHD[1] === HD);
      const symmetric = this.node.attributes.get('symmetric');
      const [H, D] = (hasWHD ? WHD : [HD, 0]);
      let [h, d] = [H + D, 0];
      if (symmetric) {
        //
        //  Center on the math axis
        //
        const a = this.font.params.axis_height;
        if (hasWHD) {
          h = 2 * Math.max(H - a, D + a);
        }
        d = h / 2 - a;
      } else if (hasWHD) {
        //
        //  Use the given depth (from mrow)
        //
        d = D;
      } else {
        //
        //  Use depth proportional to the normal-size character
        //  (when stretching for minsize or maxsize by itself)
        //
        let [ch, cd] = (C.HDW || [.75, .25]);
        d = cd * (h / (ch + cd));
      }
      return [h - d, d];
    }

    /**
     * @override
     */
    public remapChars(chars: number[]) {
      if (chars.length === 1) {
        const parent = (this.node as MmlMo).coreParent().parent;
        const isAccent = this.isAccent && !parent.isKind('mrow');
        const map = (isAccent ? 'accent' : 'mo');
        const text = this.font.getRemappedChar(map, chars[0]);
        if (text) {
          chars = this.unicodeChars(text, this.variant);
        }
      }
      return chars;
    }

  };

}

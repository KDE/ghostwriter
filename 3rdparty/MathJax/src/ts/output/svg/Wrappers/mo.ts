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
 * @fileoverview  Implements the SVGmo wrapper for the MmlMo object
 *
 * @author dpvc@mathjax.org (Davide Cervone)
 */

import {SVGWrapper, SVGConstructor} from '../Wrapper.js';
import {CommonMoMixin} from '../../common/Wrappers/mo.js';
import {MmlMo} from '../../../core/MmlTree/MmlNodes/mo.js';
import {BBox} from '../../../util/BBox.js';
import {DIRECTION, SVGCharData, SVGCharOptions} from '../FontData.js';


/*****************************************************************/

const VFUZZ = 0.1;       // overlap for vertical stretchy glyphs
const HFUZZ = 0.1;       // overlap for horizontal stretchy glyphs

/*****************************************************************/
/**
 * The SVGmo wrapper for the MmlMo object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class SVGmo<N, T, D> extends
CommonMoMixin<SVGConstructor<any, any, any>>(SVGWrapper) {

  /**
   * The mo wrapper
   */
  public static kind = MmlMo.prototype.kind;

  /**
   * @override
   */
  public toSVG(parent: N) {
    const attributes = this.node.attributes;
    const symmetric = (attributes.get('symmetric') as boolean) && this.stretch.dir !== DIRECTION.Horizontal;
    const stretchy = this.stretch.dir !== DIRECTION.None;
    if (stretchy && this.size === null) {
      this.getStretchedVariant([]);
    }
    let svg = this.standardSVGnode(parent);
    if (stretchy && this.size < 0) {
      this.stretchSVG();
    } else {
      if (symmetric || attributes.get('largeop')) {
        const bbox = BBox.empty();
        super.computeBBox(bbox);
        const u = this.fixed((bbox.d - bbox.h) / 2 + this.font.params.axis_height);
        if (u !== '0') {
          this.adaptor.setAttribute(svg, 'transform', 'translate(0 ' + u + ')');
        }
      }
      this.addChildren(svg);
    }
  }

  /**
   * Create the SVG for a multi-character stretchy delimiter
   */
  protected stretchSVG() {
    const stretch = this.stretch.stretch;
    const bbox = this.getBBox();
    if (this.stretch.dir === DIRECTION.Vertical) {
      this.stretchVertical(stretch, bbox);
    } else {
      this.stretchHorizontal(stretch, bbox);
    }
  }

  /**
   * @param {number[]} stretch    The characters to use for stretching
   * @param {BBox} bbox           The full size of the stretched character
   */
  protected stretchVertical(stretch: number[], bbox: BBox) {
    const {h, d, w} = bbox;
    const T = this.addTop(stretch[0], h, w);
    const B = this.addBot(stretch[2], d, w);
    if (stretch.length === 4) {
      const [H, D] = this.addMidV(stretch[3], w);
      this.addExtV(stretch[1], h, 0, T, H, w);
      this.addExtV(stretch[1], 0, d, D, B, w);
    } else {
      this.addExtV(stretch[1], h, d, T, B, w);
    }
  }

  /**
   * @param {number[]} stretch    The characters to use for stretching
   * @param {BBox} bbox           The full size of the stretched character
   */
  protected stretchHorizontal(stretch: number[], bbox: BBox) {
    const w = bbox.w;
    const L = this.addLeft(stretch[0]);
    const R = this.addRight(stretch[2], w);
    if (stretch.length === 4) {
      const [x1, x2] = this.addMidH(stretch[3], w);
      const w2 = w / 2;
      this.addExtH(stretch[1], w2, L, w2 - x1);
      this.addExtH(stretch[1], w2, x2 - w2, R, w2);
    } else {
      this.addExtH(stretch[1], w, L, R);
    }
  }

  /***********************************************************/

  /**
   * @param {number} n         The number of the character to look up
   * @return {SVGCharData}     The full CharData object, with CharOptions guaranteed to be defined
   */
  protected getChar(n: number): SVGCharData {
    const char = this.font.getChar('-size4', n) || [0, 0, 0, null];
    return [char[0], char[1], char[2], char[3] || {}] as [number, number, number, SVGCharOptions];
  }

  /**
   * @param {number} n   The character code for the glyph
   * @param {number} x   The x position of the glyph
   * @param {number} y   The y position of the glyph
   * @param {N} parent   The container for the glyph
   * @return {number}    The width of the character placed
   */
  protected addGlyph(n: number, x: number, y: number, parent: N = null): number {
    return this.placeChar(n, x, y, parent || this.element, '-size4');
  }

  /***********************************************************/

  /**
   * @param {number} n    The character number for the top glyph
   * @param {number} H    The height of the stretched delimiter
   * @param {number} W    The width of the stretched delimiter
   * @return {number}     The total height of the top glyph
   */
  protected addTop(n: number, H: number, W: number): number {
    if (!n)  return 0;
    const [h, d, w] = this.getChar(n);
    this.addGlyph(n, (W - w) / 2, H - h);
    return h + d;
  }

  /**
   * @param {number} n    The character number for the extender glyph
   * @param {number} H    The height of the stretched delimiter
   * @param {number} D    The depth of the stretched delimiter
   * @param {number} T    The height of the top glyph in the delimiter
   * @param {number} B    The height of the bottom glyph in the delimiter
   * @param {number} W    The width of the stretched delimiter
   */
  protected addExtV(n: number, H: number, D: number, T: number, B: number, W: number) {
    if (!n) return;
    T = Math.max(0, T - VFUZZ);              // A little overlap on top
    B = Math.max(0, B - VFUZZ);              // A little overlap on bottom
    const adaptor = this.adaptor;
    const [h, d, w] = this.getChar(n);
    const Y = H + D - T - B;                 // The height of the extender
    const s = 1.5 * Y / (h + d);             // Scale height by 1.5 to avoid bad ends
    //   (glyphs with rounded or anti-aliased ends don't stretch well,
    //    so this makes for sharper ends)
    const y = (s * (h - d) - Y) / 2;         // The bottom point to clip the extender
    if (Y <= 0) return;
    const svg = this.svg('svg', {
      width: this.fixed(w), height: this.fixed(Y),
      y: this.fixed(B - D), x: this.fixed((W - w) / 2),
      viewBox: [0, y, w, Y].map(x => this.fixed(x)).join(' ')
    });
    this.addGlyph(n, 0, 0, svg);
    const glyph = adaptor.lastChild(svg);
    adaptor.setAttribute(glyph, 'transform', 'scale(1, ' + this.jax.fixed(s) + ')');
    adaptor.append(this.element, svg);
  }

  /**
   * @param {number} n    The character number for the bottom glyph
   * @param {number} D    The depth of the stretched delimiter
   * @param {number} W    The width of the stretched delimiter
   * @return {number}     The total height of the bottom glyph
   */
  protected addBot(n: number, D: number, W: number): number {
    if (!n) return 0;
    const [h, d, w] = this.getChar(n);
    this.addGlyph(n, (W - w) / 2, d - D);
    return h + d;
  }

  /**
   * @param {number} n    The character number for the middle glyph
   * @param {number} W    The width of the stretched delimiter
   * @return {[number, number]}   The top and bottom positions of the middle glyph
   */
  protected addMidV(n: number, W: number): [number, number] {
    if (!n) return [0, 0];
    const [h, d, w] = this.getChar(n);
    const y = (d - h) / 2 + this.font.params.axis_height;
    this.addGlyph(n, (W - w) / 2, y);
    return [h + y, d - y];
  }

  /***********************************************************/

  /**
   * @param {number} n   The character number for the left glyph of the stretchy character
   * @return {number}    The width of the left glyph
   */
  protected addLeft(n: number): number {
    return (n ? this.addGlyph(n, 0, 0) : 0);
  }

  /**
   * @param {number} n   The character number for the extender glyph of the stretchy character
   * @param {number} W   The width of the stretched character
   * @param {number} L   The width of the left glyph of the stretchy character
   * @param {number} R   The width of the right glyph of the stretchy character
   * @param {number} x   The x-position of the extender (needed for ones with two extenders)
   */
  protected addExtH(n: number, W: number, L: number, R: number, x: number = 0) {
    if (!n) return;
    R = Math.max(0, R - HFUZZ);     // A little less than the width of the right glyph
    L = Math.max(0, L - HFUZZ);     // A little less than the width of the left glyph
    const [h, d, w] = this.getChar(n);
    const X = W - L - R;            // The width of the extender
    const Y = h + d + 2 * VFUZZ;    // The height (plus some fuzz) of the extender
    const s = 1.5 * (X / w);        // Scale the width so that left- and right-bearing won't hurt us
    const D = -(d + VFUZZ);         // The bottom position of the glyph
    if (X <= 0) return;
    const svg = this.svg('svg', {
      width: this.fixed(X), height: this.fixed(Y),
      x: this.fixed(x + L), y: this.fixed(D),
      viewBox: [(s * w - X) / 2, D, X, Y].map(x => this.fixed(x)).join(' ')
    });
    this.addGlyph(n, 0, 0, svg);
    const glyph = this.adaptor.lastChild(svg);
    this.adaptor.setAttribute(glyph, 'transform', 'scale(' + this.jax.fixed(s) + ', 1)');
    this.adaptor.append(this.element, svg);
  }

  /**
   * @param {number} n   The character number for the right glyph of the stretchy character
   * @param {number} W   The width of the stretched character
   * @return {number}    The width of the right glyph
   */
  protected addRight(n: number, W: number): number {
    if (!n) return 0;
    const w = this.getChar(n)[2];
    return this.addGlyph(n, W - w, 0);
  }

  /**
   * @param {number} n   The character number for the middle glyph of the stretchy character
   * @param {number} W   The width of the stretched character
   * @return {[number, number]}  The positions of the left and right edges of the middle glyph
   */
  protected addMidH(n: number, W: number): [number, number] {
    if (!n) return [0, 0];
    const w = this.getChar(n)[2];
    this.addGlyph(n, (W - w) / 2, 0);
    return [(W - w) / 2, (W + w) / 2];
  }

}

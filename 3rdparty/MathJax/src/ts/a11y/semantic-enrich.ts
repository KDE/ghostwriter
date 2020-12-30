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
 * @fileoverview  Mixin that adds semantic enrichment to internal MathML
 *
 * @author dpvc@mathjax.org (Davide Cervone)
 */

import {mathjax} from '../mathjax.js';
import {Handler} from '../core/Handler.js';
import {MathDocument, AbstractMathDocument, MathDocumentConstructor} from '../core/MathDocument.js';
import {MathItem, AbstractMathItem, STATE, newState} from '../core/MathItem.js';
import {MmlNode} from '../core/MmlTree/MmlNode.js';
import {MathML} from '../input/mathml.js';
import {SerializedMmlVisitor} from '../core/MmlTree/SerializedMmlVisitor.js';
import {OptionList, expandable} from '../util/Options.js';

import {sreReady} from './sre.js';

/*==========================================================================*/

/**
 *  The current speech setting for SRE
 */
let currentSpeech = 'none';

/**
 * Generic constructor for Mixins
 */
export type Constructor<T> = new(...args: any[]) => T;

/*==========================================================================*/

/**
 * Add STATE value for being enriched (after COMPILED and before TYPESET)
 */
newState('ENRICHED', 30);

/**
 * Add STATE value for adding speech (after TYPESET)
 */
newState('ATTACHSPEECH', 155);


/**
 * The funtions added to MathItem for enrichment
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
export interface EnrichedMathItem<N, T, D> extends MathItem<N, T, D> {

  /**
   * @param {MathDocument} document  The document where enrichment is occurring
   * @param {boolean} force          True to force the enrichment even if not enabled
   */
  enrich(document: MathDocument<N, T, D>, force?: boolean): void;

  /**
   * @param {MathDocument} document  The document where enrichment is occurring
   */
  attachSpeech(document: MathDocument<N, T, D>): void;
}

/**
 * The mixin for adding enrichment to MathItems
 *
 * @param {B} BaseMathItem     The MathItem class to be extended
 * @param {MathML} MmlJax      The MathML input jax used to convert the enriched MathML
 * @param {Function} toMathML  The function to serialize the internal MathML
 * @return {EnrichedMathItem}  The enriched MathItem class
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 * @template B  The MathItem class to extend
 */
export function EnrichedMathItemMixin<N, T, D, B extends Constructor<AbstractMathItem<N, T, D>>>(
  BaseMathItem: B,
  MmlJax: MathML<N, T, D>,
  toMathML: (node: MmlNode) => string
): Constructor<EnrichedMathItem<N, T, D>> & B {

  return class extends BaseMathItem {

    /**
     * @param {any} node  The node to be serialized
     * @return {string}   The serialized version of node
     */
    protected serializeMml(node: any): string {
      if ('outerHTML' in node) {
        return node.outerHTML;
      }
      //
      //  For IE11
      //
      if (typeof Element !== 'undefined' && typeof window !== 'undefined' && node instanceof Element) {
        const div = window.document.createElement('div');
        div.appendChild(node);
        return div.innerHTML;
      }
      //
      //  For NodeJS version of SRE
      //
      return node.toString();
    }

    /**
     * @param {MathDocument} document   The MathDocument for the MathItem
     * @param {boolean} force           True to force the enrichment even if not enabled
     */
    public enrich(document: MathDocument<N, T, D>, force: boolean = false) {
      if (this.state() >= STATE.ENRICHED) return;
      if (!this.isEscaped && (document.options.enableEnrichment || force)) {
        if (typeof sre === 'undefined' || !sre.Engine.isReady()) {
          mathjax.retryAfter(sreReady());
        }
        if (document.options.enrichSpeech !== currentSpeech) {
          SRE.setupEngine({speech: document.options.enrichSpeech});
          currentSpeech = document.options.enrichSpeech;
        }
        const math = new document.options.MathItem('', MmlJax);
        math.math = this.serializeMml(SRE.toEnriched(toMathML(this.root)));
        math.display = this.display;
        math.compile(document);
        this.root = math.root;
        this.inputData.originalMml = math.math;
      }
      this.state(STATE.ENRICHED);
    }

    /**
     * @param {MathDocument} document   The MathDocument for the MathItem
     */
    public attachSpeech(document: MathDocument<N, T, D>) {
      if (this.state() >= STATE.ATTACHSPEECH) return;
      const attributes = this.root.attributes;
      const speech = (attributes.get('aria-label') ||
                      this.getSpeech(this.root)) as string;
      if (speech) {
        const adaptor = document.adaptor;
        const node = this.typesetRoot;
        adaptor.setAttribute(node, 'aria-label', speech);
        for (const child of adaptor.childNodes(node) as N[]) {
          adaptor.setAttribute(child, 'aria-hidden', 'true');
        }
      }
      this.state(STATE.ATTACHSPEECH);
    }

    /**
     * Retrieves the actual speech element that should be used as aria label.
     * @param {MmlNode} node The root node to search from.
     * @return {string} The speech content.
     */
    private getSpeech(node: MmlNode): string {
      const attributes = node.attributes;
      if (!attributes) return '';
      const speech = attributes.getExplicit('data-semantic-speech') as string;
      if (!attributes.getExplicit('data-semantic-parent') && speech) {
        return speech;
      }
      for (let child of node.childNodes) {
        let value = this.getSpeech(child as MmlNode);
        if (value != null) {
          return value;
        }
      }
      return '';
    }

  };

}

/*==========================================================================*/

/**
 * The funtions added to MathDocument for enrichment
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
export interface EnrichedMathDocument<N, T, D> extends AbstractMathDocument<N, T, D> {

  /**
   * Perform enrichment on the MathItems in the MathDocument
   *
   * @return {EnrichedMathDocument}   The MathDocument (so calls can be chained)
   */
  enrich(): EnrichedMathDocument<N, T, D>;

  /**
   * Attach speech to the MathItems in the MathDocument
   *
   * @return {EnrichedMathDocument}   The MathDocument (so calls can be chained)
   */
  attachSpeech(): EnrichedMathDocument<N, T, D>;
}

/**
 * The mixin for adding enrichment to MathDocuments
 *
 * @param {B} BaseDocument     The MathDocument class to be extended
 * @param {MathML} MmlJax          The MathML input jax used to convert the enriched MathML
 * @return {EnrichedMathDocument}  The enriched MathDocument class
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 * @template B  The MathDocument class to extend
 */
export function EnrichedMathDocumentMixin<N, T, D, B extends MathDocumentConstructor<AbstractMathDocument<N, T, D>>>(
  BaseDocument: B,
  MmlJax: MathML<N, T, D>,
): MathDocumentConstructor<EnrichedMathDocument<N, T, D>> & B {

  return class extends BaseDocument {

    /**
     * @override
     */
    public static OPTIONS: OptionList = {
      ...BaseDocument.OPTIONS,
      enableEnrichment: true,
      enrichSpeech: 'none',                   // or 'shallow', or 'deep'
      renderActions: expandable({
        ...BaseDocument.OPTIONS.renderActions,
        enrich:       [STATE.ENRICHED],
        attachSpeech: [STATE.ATTACHSPEECH]
      })
    };

    /**
     * Enrich the MathItem class used for this MathDocument, and create the
     *   temporary MathItem used for enrchment
     *
     * @override
     * @constructor
     */
    constructor(...args: any[]) {
      super(...args);
      MmlJax.setMmlFactory(this.mmlFactory);
      const ProcessBits = (this.constructor as typeof AbstractMathDocument).ProcessBits;
      if (!ProcessBits.has('enriched')) {
        ProcessBits.allocate('enriched');
        ProcessBits.allocate('attach-speech');
      }
      const visitor = new SerializedMmlVisitor(this.mmlFactory);
      const toMathML = ((node: MmlNode) => visitor.visitTree(node));
      this.options.MathItem =
        EnrichedMathItemMixin<N, T, D, Constructor<AbstractMathItem<N, T, D>>>(
          this.options.MathItem, MmlJax, toMathML
        );
    }

    /**
     * Attach speech from a MathItem to a node
     */
    public attachSpeech() {
      if (!this.processed.isSet('attach-speech')) {
        for (const math of this.math) {
          (math as EnrichedMathItem<N, T, D>).attachSpeech(this);
        }
        this.processed.set('attach-speech');
      }
      return this;
    }

    /**
     * Enrich the MathItems in this MathDocument
     */
    public enrich() {
      if (!this.processed.isSet('enriched')) {
        if (this.options.enableEnrichment) {
          for (const math of this.math) {
            (math as EnrichedMathItem<N, T, D>).enrich(this);
          }
        }
        this.processed.set('enriched');
      }
      return this;
    }

    /**
     * @override
     */
    public state(state: number, restore: boolean = false) {
      super.state(state, restore);
      if (state < STATE.ENRICHED) {
        this.processed.clear('enriched');
      }
      return this;
    }

  };

}

/*==========================================================================*/

/**
 * Add enrichment a Handler instance
 *
 * @param {Handler} handler   The Handler instance to enhance
 * @param {MathML} MmlJax     The MathML input jax to use for reading the enriched MathML
 * @return {Handler}          The handler that was modified (for purposes of chainging extensions)
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
export function EnrichHandler<N, T, D>(handler: Handler<N, T, D>, MmlJax: MathML<N, T, D>): Handler<N, T, D> {
  MmlJax.setAdaptor(handler.adaptor);
  handler.documentClass =
    EnrichedMathDocumentMixin<N, T, D, MathDocumentConstructor<AbstractMathDocument<N, T, D>>>(
      handler.documentClass, MmlJax
    );
  return handler;
}

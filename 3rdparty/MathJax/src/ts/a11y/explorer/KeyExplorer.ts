/*************************************************************
 *
 *  Copyright (c) 2009-2019 The MathJax Consortium
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
 * @fileoverview Explorers based on keyboard events.
 *
 * @author v.sorge@mathjax.org (Volker Sorge)
 */


import {A11yDocument, Region} from './Region.js';
import {Explorer, AbstractExplorer} from './Explorer.js';
import {sreReady} from '../sre.js';


/**
 * Interface for keyboard explorers. Adds the necessary keyboard events.
 * @interface
 * @extends {Explorer}
 */
export interface KeyExplorer extends Explorer {

  /**
   * Function to be executed on key down.
   * @param {KeyboardEvent} event The keyboard event.
   */
  KeyDown(event: KeyboardEvent): void;

  /**
   * Function to be executed on focus in.
   * @param {KeyboardEvent} event The keyboard event.
   */
  FocusIn(event: FocusEvent): void;

  /**
   * Function to be executed on focus out.
   * @param {KeyboardEvent} event The keyboard event.
   */
  FocusOut(event: FocusEvent): void;

}


/**
 * @constructor
 * @extends {AbstractExplorer}
 *
 * @template T  The type that is consumed by the Region of this explorer.
 */
export abstract class AbstractKeyExplorer<T> extends AbstractExplorer<T> implements KeyExplorer {

  /**
   * The attached SRE walker.
   * @type {sre.Walker}
   */
  protected walker: sre.Walker;

  /**
   * @override
   */
  protected events: [string, (x: Event) => void][] =
    super.Events().concat(
      [['keydown', this.KeyDown.bind(this)],
       ['focusin', this.FocusIn.bind(this)],
       ['focusout', this.FocusOut.bind(this)]]);

  /**
   * The original tabindex value before explorer was attached.
   * @type {boolean}
   */
  private oldIndex: number = null;

  /**
   * @override
   */
  public abstract KeyDown(event: KeyboardEvent): void;

  /**
   * @override
   */
  public FocusIn(_event: FocusEvent) {
  }

  /**
   * @override
   */
  public FocusOut(_event: FocusEvent) {
    this.Stop();
  }

  /**
   * @override
   */
  public Update(force: boolean = false) {
    if (!this.active && !force) return;
    this.highlighter.unhighlight();
    this.highlighter.highlight(this.walker.getFocus(true).getNodes());
  }

  /**
   * @override
   */
  public Attach() {
    super.Attach();
    this.oldIndex = this.node.tabIndex;
    this.node.tabIndex = 1;
    this.node.setAttribute('role', 'application');
  }

  /**
   * @override
   */
  public Detach() {
    this.node.tabIndex = this.oldIndex;
    this.oldIndex = null;
    this.node.removeAttribute('role');
    super.Detach();
  }

  /**
   * @override
   */
  public Stop() {
    if (this.active) {
      this.highlighter.unhighlight();
      this.walker.deactivate();
    }
    super.Stop();
  }

}


/**
 * Explorer that pushes speech to live region.
 * @constructor
 * @extends {AbstractKeyExplorer}
 */
export class SpeechExplorer extends AbstractKeyExplorer<string> {

  /**
   * The SRE speech generator associated with the walker.
   * @type {sre.SpeechGenerator}
   */
  public speechGenerator: sre.SpeechGenerator;

  /**
   * The name of the option used to control when this is being shown
   * @type {string}
   */
  public showRegion: string = 'subtitles';

  private init: boolean = false;

  /**
   * Flag in case the start method is triggered before the walker is fully
   * initialised. I.e., we have to wait for SRE. Then region is re-shown if
   * necessary, as otherwise it leads to incorrect stacking.
   * @type {boolean}
   */
  private restarted: boolean = false;

  /**
   * @constructor
   * @extends {AbstractKeyExplorer}
   */
  constructor(public document: A11yDocument,
              protected region: Region<string>,
              protected node: HTMLElement,
              private mml: HTMLElement) {
    super(document, region, node);
    this.initWalker();
  }


  /**
   * @override
   */
  public Start() {
    let options = this.getOptions();
    // TODO: Check and set locale not only on init, but on every start.
    if (!this.init) {
      this.init = true;
      sreReady().then(() => {
        if (SRE.engineSetup().locale !== options.locale) {
          SRE.setupEngine({locale: options.locale});
        }
        sreReady().then(() => {
          this.Speech(this.walker);
          this.Start();
        });
      }).catch((error: Error) => console.log(error.message));
      return;
    }
    super.Start();
    this.speechGenerator = sre.SpeechGeneratorFactory.generator('Direct');
    this.speechGenerator.setOptions(options);
    this.walker = sre.WalkerFactory.walker('table',
                                           this.node, this.speechGenerator, this.highlighter, this.mml);
    this.walker.activate();
    this.Update();
    if (this.document.options.a11y[this.showRegion]) {
      this.region.Show(this.node, this.highlighter);
    }
    this.restarted = true;
  }


  /**
   * @override
   */
  public Update(force: boolean = false) {
    super.Update(force);
    this.region.Update(this.walker.speech());
    // This is a necessary in case speech options have changed via keypress
    // during walking.
    let options = this.speechGenerator.getOptions();
    if (options.modality === 'speech') {
      this.document.options.a11y.speechRules = options.domain + '-' + options.style;
    }
  }


  /**
   * Computes the speech for the current expression once SRE is ready.
   * @param {sre.Walker} walker The sre walker.
   */
  public Speech(walker: sre.Walker) {
    walker.speech();
    this.node.setAttribute('hasspeech', 'true');
    this.Update();
    if (this.restarted && this.document.options.a11y[this.showRegion]) {
      this.region.Show(this.node, this.highlighter);
    }
  }


  /**
   * @override
   */
  public KeyDown(event: KeyboardEvent) {
    const code = event.keyCode;
    if (code === 27) {
      this.Stop();
      this.stopEvent(event);
      return;
    }
    if (this.active) {
      this.Move(code);
      this.stopEvent(event);
      return;
    }
    if (code === 32 && event.shiftKey || code === 13) {
      this.Start();
      this.stopEvent(event);
    }
  }


  /**
   * @override
   */
  public Move(key: number) {
    this.walker.move(key);
    this.Update();
  }

  /**
   * Initialises the SRE walker.
   */
  private initWalker() {
    this.speechGenerator = sre.SpeechGeneratorFactory.generator('Tree');
    let dummy = sre.WalkerFactory.walker(
      'dummy', this.node, this.speechGenerator, this.highlighter, this.mml);
    this.walker = dummy;
  }

  /**
   * Retrieves the speech options to sync with document options.
   * @return {{[key: string]: string}} The options settings for the speech
   *     generator.
   */
  private getOptions(): {[key: string]: string} {
    let options = this.speechGenerator.getOptions();
    let [domain, style] = this.document.options.a11y.speechRules.split('-');
    if (options.modality === 'speech' &&
        (options.locale !== this.document.options.a11y.locale ||
          options.domain !== domain || options.style !== style)) {
      options.domain = domain;
      options.style = style;
      options.locale = this.document.options.a11y.locale;
      this.walker.update(options);
    }
    return options;
  }

}


/**
 * Explorer that magnifies what is currently explored. Uses a hover region.
 * @constructor
 * @extends {AbstractKeyExplorer}
 */
export class Magnifier extends AbstractKeyExplorer<HTMLElement> {

  /**
   * @constructor
   * @extends {AbstractKeyExplorer}
   */
  constructor(public document: A11yDocument,
              protected region: Region<HTMLElement>,
              protected node: HTMLElement,
              private mml: HTMLElement) {
    super(document, region, node);
    this.walker = sre.WalkerFactory.walker(
      'table', this.node, sre.SpeechGeneratorFactory.generator('Dummy'),
      this.highlighter, this.mml);
  }

  /**
   * @override
   */
  public Update(force: boolean = false) {
    super.Update(force);
    this.showFocus();
  }


  /**
   * @override
   */
  public Start() {
    super.Start();
    this.region.Show(this.node, this.highlighter);
    this.walker.activate();
    this.Update();
  }


  /**
   * Shows the nodes that are currently focused.
   */
  private showFocus() {
    let node = this.walker.getFocus().getNodes()[0] as HTMLElement;
    this.region.Show(node, this.highlighter);
  }


  /**
   * @override
   */
  public Move(key: number) {
    let result = this.walker.move(key);
    if (result) {
      this.Update();
    }
  }


  /**
   * @override
   */
  public KeyDown(event: KeyboardEvent) {
    const code = event.keyCode;
    if (code === 27) {
      this.Stop();
      this.stopEvent(event);
      return;
    }
    if (this.active && code !== 13) {
      this.Move(code);
      this.stopEvent(event);
      return;
    }
    if (code === 32 && event.shiftKey || code === 13) {
      this.Start();
      this.stopEvent(event);
    }
  }

}

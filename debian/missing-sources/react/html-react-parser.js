(function (global, factory) {
  typeof exports === 'object' && typeof module !== 'undefined' ? module.exports = factory(require('react')) :
  typeof define === 'function' && define.amd ? define(['react'], factory) :
  (global = typeof globalThis !== 'undefined' ? globalThis : global || self, global.HTMLReactParser = factory(global.React));
}(this, (function (React) { 'use strict';

  function _interopDefaultLegacy (e) { return e && typeof e === 'object' && 'default' in e ? e : { 'default': e }; }

  var React__default = /*#__PURE__*/_interopDefaultLegacy(React);

  var HTMLDOMPropertyConfig = {
    Properties: {
      autoFocus: 4,
      accept: 0,
      acceptCharset: 0,
      accessKey: 0,
      action: 0,
      allowFullScreen: 4,
      allowTransparency: 0,
      alt: 0,
      as: 0,
      async: 4,
      autoComplete: 0,
      autoPlay: 4,
      capture: 4,
      cellPadding: 0,
      cellSpacing: 0,
      charSet: 0,
      challenge: 0,
      checked: 5,
      cite: 0,
      classID: 0,
      className: 0,
      cols: 24,
      colSpan: 0,
      content: 0,
      contentEditable: 0,
      contextMenu: 0,
      controls: 4,
      controlsList: 0,
      coords: 0,
      crossOrigin: 0,
      data: 0,
      dateTime: 0,
      default: 4,
      defer: 4,
      dir: 0,
      disabled: 4,
      download: 32,
      draggable: 0,
      encType: 0,
      form: 0,
      formAction: 0,
      formEncType: 0,
      formMethod: 0,
      formNoValidate: 4,
      formTarget: 0,
      frameBorder: 0,
      headers: 0,
      height: 0,
      hidden: 4,
      high: 0,
      href: 0,
      hrefLang: 0,
      htmlFor: 0,
      httpEquiv: 0,
      icon: 0,
      id: 0,
      inputMode: 0,
      integrity: 0,
      is: 0,
      keyParams: 0,
      keyType: 0,
      kind: 0,
      label: 0,
      lang: 0,
      list: 0,
      loop: 4,
      low: 0,
      manifest: 0,
      marginHeight: 0,
      marginWidth: 0,
      max: 0,
      maxLength: 0,
      media: 0,
      mediaGroup: 0,
      method: 0,
      min: 0,
      minLength: 0,
      multiple: 5,
      muted: 5,
      name: 0,
      nonce: 0,
      noValidate: 4,
      open: 4,
      optimum: 0,
      pattern: 0,
      placeholder: 0,
      playsInline: 4,
      poster: 0,
      preload: 0,
      profile: 0,
      radioGroup: 0,
      readOnly: 4,
      referrerPolicy: 0,
      rel: 0,
      required: 4,
      reversed: 4,
      role: 0,
      rows: 24,
      rowSpan: 8,
      sandbox: 0,
      scope: 0,
      scoped: 4,
      scrolling: 0,
      seamless: 4,
      selected: 5,
      shape: 0,
      size: 24,
      sizes: 0,
      span: 24,
      spellCheck: 0,
      src: 0,
      srcDoc: 0,
      srcLang: 0,
      srcSet: 0,
      start: 8,
      step: 0,
      style: 0,
      summary: 0,
      tabIndex: 0,
      target: 0,
      title: 0,
      type: 0,
      useMap: 0,
      value: 0,
      width: 0,
      wmode: 0,
      wrap: 0,
      about: 0,
      datatype: 0,
      inlist: 0,
      prefix: 0,
      property: 0,
      resource: 0,
      typeof: 0,
      vocab: 0,
      autoCapitalize: 0,
      autoCorrect: 0,
      autoSave: 0,
      color: 0,
      itemProp: 0,
      itemScope: 4,
      itemType: 0,
      itemID: 0,
      itemRef: 0,
      results: 0,
      security: 0,
      unselectable: 0
    },
    DOMAttributeNames: {
      acceptCharset: 'accept-charset',
      className: 'class',
      htmlFor: 'for',
      httpEquiv: 'http-equiv'
    }
  };

  var SVGDOMPropertyConfig = {
    Properties: {
      accentHeight: 0,
      accumulate: 0,
      additive: 0,
      alignmentBaseline: 0,
      allowReorder: 0,
      alphabetic: 0,
      amplitude: 0,
      arabicForm: 0,
      ascent: 0,
      attributeName: 0,
      attributeType: 0,
      autoReverse: 0,
      azimuth: 0,
      baseFrequency: 0,
      baseProfile: 0,
      baselineShift: 0,
      bbox: 0,
      begin: 0,
      bias: 0,
      by: 0,
      calcMode: 0,
      capHeight: 0,
      clip: 0,
      clipPath: 0,
      clipRule: 0,
      clipPathUnits: 0,
      colorInterpolation: 0,
      colorInterpolationFilters: 0,
      colorProfile: 0,
      colorRendering: 0,
      contentScriptType: 0,
      contentStyleType: 0,
      cursor: 0,
      cx: 0,
      cy: 0,
      d: 0,
      decelerate: 0,
      descent: 0,
      diffuseConstant: 0,
      direction: 0,
      display: 0,
      divisor: 0,
      dominantBaseline: 0,
      dur: 0,
      dx: 0,
      dy: 0,
      edgeMode: 0,
      elevation: 0,
      enableBackground: 0,
      end: 0,
      exponent: 0,
      externalResourcesRequired: 0,
      fill: 0,
      fillOpacity: 0,
      fillRule: 0,
      filter: 0,
      filterRes: 0,
      filterUnits: 0,
      floodColor: 0,
      floodOpacity: 0,
      focusable: 0,
      fontFamily: 0,
      fontSize: 0,
      fontSizeAdjust: 0,
      fontStretch: 0,
      fontStyle: 0,
      fontVariant: 0,
      fontWeight: 0,
      format: 0,
      from: 0,
      fx: 0,
      fy: 0,
      g1: 0,
      g2: 0,
      glyphName: 0,
      glyphOrientationHorizontal: 0,
      glyphOrientationVertical: 0,
      glyphRef: 0,
      gradientTransform: 0,
      gradientUnits: 0,
      hanging: 0,
      horizAdvX: 0,
      horizOriginX: 0,
      ideographic: 0,
      imageRendering: 0,
      in: 0,
      in2: 0,
      intercept: 0,
      k: 0,
      k1: 0,
      k2: 0,
      k3: 0,
      k4: 0,
      kernelMatrix: 0,
      kernelUnitLength: 0,
      kerning: 0,
      keyPoints: 0,
      keySplines: 0,
      keyTimes: 0,
      lengthAdjust: 0,
      letterSpacing: 0,
      lightingColor: 0,
      limitingConeAngle: 0,
      local: 0,
      markerEnd: 0,
      markerMid: 0,
      markerStart: 0,
      markerHeight: 0,
      markerUnits: 0,
      markerWidth: 0,
      mask: 0,
      maskContentUnits: 0,
      maskUnits: 0,
      mathematical: 0,
      mode: 0,
      numOctaves: 0,
      offset: 0,
      opacity: 0,
      operator: 0,
      order: 0,
      orient: 0,
      orientation: 0,
      origin: 0,
      overflow: 0,
      overlinePosition: 0,
      overlineThickness: 0,
      paintOrder: 0,
      panose1: 0,
      pathLength: 0,
      patternContentUnits: 0,
      patternTransform: 0,
      patternUnits: 0,
      pointerEvents: 0,
      points: 0,
      pointsAtX: 0,
      pointsAtY: 0,
      pointsAtZ: 0,
      preserveAlpha: 0,
      preserveAspectRatio: 0,
      primitiveUnits: 0,
      r: 0,
      radius: 0,
      refX: 0,
      refY: 0,
      renderingIntent: 0,
      repeatCount: 0,
      repeatDur: 0,
      requiredExtensions: 0,
      requiredFeatures: 0,
      restart: 0,
      result: 0,
      rotate: 0,
      rx: 0,
      ry: 0,
      scale: 0,
      seed: 0,
      shapeRendering: 0,
      slope: 0,
      spacing: 0,
      specularConstant: 0,
      specularExponent: 0,
      speed: 0,
      spreadMethod: 0,
      startOffset: 0,
      stdDeviation: 0,
      stemh: 0,
      stemv: 0,
      stitchTiles: 0,
      stopColor: 0,
      stopOpacity: 0,
      strikethroughPosition: 0,
      strikethroughThickness: 0,
      string: 0,
      stroke: 0,
      strokeDasharray: 0,
      strokeDashoffset: 0,
      strokeLinecap: 0,
      strokeLinejoin: 0,
      strokeMiterlimit: 0,
      strokeOpacity: 0,
      strokeWidth: 0,
      surfaceScale: 0,
      systemLanguage: 0,
      tableValues: 0,
      targetX: 0,
      targetY: 0,
      textAnchor: 0,
      textDecoration: 0,
      textRendering: 0,
      textLength: 0,
      to: 0,
      transform: 0,
      u1: 0,
      u2: 0,
      underlinePosition: 0,
      underlineThickness: 0,
      unicode: 0,
      unicodeBidi: 0,
      unicodeRange: 0,
      unitsPerEm: 0,
      vAlphabetic: 0,
      vHanging: 0,
      vIdeographic: 0,
      vMathematical: 0,
      values: 0,
      vectorEffect: 0,
      version: 0,
      vertAdvY: 0,
      vertOriginX: 0,
      vertOriginY: 0,
      viewBox: 0,
      viewTarget: 0,
      visibility: 0,
      widths: 0,
      wordSpacing: 0,
      writingMode: 0,
      x: 0,
      xHeight: 0,
      x1: 0,
      x2: 0,
      xChannelSelector: 0,
      xlinkActuate: 0,
      xlinkArcrole: 0,
      xlinkHref: 0,
      xlinkRole: 0,
      xlinkShow: 0,
      xlinkTitle: 0,
      xlinkType: 0,
      xmlBase: 0,
      xmlns: 0,
      xmlnsXlink: 0,
      xmlLang: 0,
      xmlSpace: 0,
      y: 0,
      y1: 0,
      y2: 0,
      yChannelSelector: 0,
      z: 0,
      zoomAndPan: 0
    },
    DOMAttributeNames: {
      accentHeight: 'accent-height',
      alignmentBaseline: 'alignment-baseline',
      arabicForm: 'arabic-form',
      baselineShift: 'baseline-shift',
      capHeight: 'cap-height',
      clipPath: 'clip-path',
      clipRule: 'clip-rule',
      colorInterpolation: 'color-interpolation',
      colorInterpolationFilters: 'color-interpolation-filters',
      colorProfile: 'color-profile',
      colorRendering: 'color-rendering',
      dominantBaseline: 'dominant-baseline',
      enableBackground: 'enable-background',
      fillOpacity: 'fill-opacity',
      fillRule: 'fill-rule',
      floodColor: 'flood-color',
      floodOpacity: 'flood-opacity',
      fontFamily: 'font-family',
      fontSize: 'font-size',
      fontSizeAdjust: 'font-size-adjust',
      fontStretch: 'font-stretch',
      fontStyle: 'font-style',
      fontVariant: 'font-variant',
      fontWeight: 'font-weight',
      glyphName: 'glyph-name',
      glyphOrientationHorizontal: 'glyph-orientation-horizontal',
      glyphOrientationVertical: 'glyph-orientation-vertical',
      horizAdvX: 'horiz-adv-x',
      horizOriginX: 'horiz-origin-x',
      imageRendering: 'image-rendering',
      letterSpacing: 'letter-spacing',
      lightingColor: 'lighting-color',
      markerEnd: 'marker-end',
      markerMid: 'marker-mid',
      markerStart: 'marker-start',
      overlinePosition: 'overline-position',
      overlineThickness: 'overline-thickness',
      paintOrder: 'paint-order',
      panose1: 'panose-1',
      pointerEvents: 'pointer-events',
      renderingIntent: 'rendering-intent',
      shapeRendering: 'shape-rendering',
      stopColor: 'stop-color',
      stopOpacity: 'stop-opacity',
      strikethroughPosition: 'strikethrough-position',
      strikethroughThickness: 'strikethrough-thickness',
      strokeDasharray: 'stroke-dasharray',
      strokeDashoffset: 'stroke-dashoffset',
      strokeLinecap: 'stroke-linecap',
      strokeLinejoin: 'stroke-linejoin',
      strokeMiterlimit: 'stroke-miterlimit',
      strokeOpacity: 'stroke-opacity',
      strokeWidth: 'stroke-width',
      textAnchor: 'text-anchor',
      textDecoration: 'text-decoration',
      textRendering: 'text-rendering',
      underlinePosition: 'underline-position',
      underlineThickness: 'underline-thickness',
      unicodeBidi: 'unicode-bidi',
      unicodeRange: 'unicode-range',
      unitsPerEm: 'units-per-em',
      vAlphabetic: 'v-alphabetic',
      vHanging: 'v-hanging',
      vIdeographic: 'v-ideographic',
      vMathematical: 'v-mathematical',
      vectorEffect: 'vector-effect',
      vertAdvY: 'vert-adv-y',
      vertOriginX: 'vert-origin-x',
      vertOriginY: 'vert-origin-y',
      wordSpacing: 'word-spacing',
      writingMode: 'writing-mode',
      xHeight: 'x-height',
      xlinkActuate: 'xlink:actuate',
      xlinkArcrole: 'xlink:arcrole',
      xlinkHref: 'xlink:href',
      xlinkRole: 'xlink:role',
      xlinkShow: 'xlink:show',
      xlinkTitle: 'xlink:title',
      xlinkType: 'xlink:type',
      xmlBase: 'xml:base',
      xmlnsXlink: 'xmlns:xlink',
      xmlLang: 'xml:lang',
      xmlSpace: 'xml:space'
    }
  };

  var injection = {
    MUST_USE_PROPERTY: 1,
    HAS_BOOLEAN_VALUE: 4,
    HAS_NUMERIC_VALUE: 8,
    HAS_POSITIVE_NUMERIC_VALUE: 24,
    HAS_OVERLOADED_BOOLEAN_VALUE: 32
  };

  var MUST_USE_PROPERTY = injection.MUST_USE_PROPERTY;
  var HAS_BOOLEAN_VALUE = injection.HAS_BOOLEAN_VALUE;
  var HAS_NUMERIC_VALUE = injection.HAS_NUMERIC_VALUE;
  var HAS_POSITIVE_NUMERIC_VALUE = injection.HAS_POSITIVE_NUMERIC_VALUE;
  var HAS_OVERLOADED_BOOLEAN_VALUE = injection.HAS_OVERLOADED_BOOLEAN_VALUE;

  /**
   * @see https://github.com/facebook/react/blob/15-stable/src/renderers/dom/shared/DOMProperty.js#L14-L16
   *
   * @param  {Number}  value
   * @param  {Number}  bitmask
   * @return {Boolean}
   */
  function checkMask(value, bitmask) {
    return (value & bitmask) === bitmask;
  }

  /**
   * @see https://github.com/facebook/react/blob/15-stable/src/renderers/dom/shared/DOMProperty.js#L57
   *
   * @param {Object}  domPropertyConfig - HTMLDOMPropertyConfig or SVGDOMPropertyConfig
   * @param {Object}  config            - The object to be mutated
   * @param {Boolean} isSVG             - Whether the injected config is HTML or SVG (it assumes the default is HTML)
   */
  function injectDOMPropertyConfig(domPropertyConfig, config, isSVG) {
    var Properties = domPropertyConfig.Properties;
    var DOMAttributeNames = domPropertyConfig.DOMAttributeNames;
    var attributeName;
    var propertyName;
    var propConfig;

    for (propertyName in Properties) {
      attributeName =
        DOMAttributeNames[propertyName] ||
        (isSVG ? propertyName : propertyName.toLowerCase());
      propConfig = Properties[propertyName];

      config[attributeName] = {
        attributeName: attributeName,
        propertyName: propertyName,
        mustUseProperty: checkMask(propConfig, MUST_USE_PROPERTY),
        hasBooleanValue: checkMask(propConfig, HAS_BOOLEAN_VALUE),
        hasNumericValue: checkMask(propConfig, HAS_NUMERIC_VALUE),
        hasPositiveNumericValue: checkMask(
          propConfig,
          HAS_POSITIVE_NUMERIC_VALUE
        ),
        hasOverloadedBooleanValue: checkMask(
          propConfig,
          HAS_OVERLOADED_BOOLEAN_VALUE
        )
      };
    }
  }

  /**
   * HTML properties config.
   *
   * @type {Object}
   */
  var html = {};
  injectDOMPropertyConfig(HTMLDOMPropertyConfig, html);

  /**
   * SVG properties config.
   *
   * @type {Object}
   */
  var svg = {};
  injectDOMPropertyConfig(SVGDOMPropertyConfig, svg, true);

  /**
   * HTML and SVG properties config.
   *
   * @type {Object}
   */
  var properties = {};
  injectDOMPropertyConfig(HTMLDOMPropertyConfig, properties);
  injectDOMPropertyConfig(SVGDOMPropertyConfig, properties, true);

  var ATTRIBUTE_NAME_START_CHAR =
    ':A-Z_a-z\\u00C0-\\u00D6\\u00D8-\\u00F6\\u00F8-\\u02FF\\u0370-\\u037D\\u037F-\\u1FFF\\u200C-\\u200D\\u2070-\\u218F\\u2C00-\\u2FEF\\u3001-\\uD7FF\\uF900-\\uFDCF\\uFDF0-\\uFFFD';
  var ATTRIBUTE_NAME_CHAR =
    ATTRIBUTE_NAME_START_CHAR + '\\-.0-9\\u00B7\\u0300-\\u036F\\u203F-\\u2040';

  var reactProperty = {
    html: html,
    svg: svg,
    properties: properties,

    /**
     * Checks whether a property name is a custom attribute.
     *
     * @see https://github.com/facebook/react/blob/15-stable/src/renderers/dom/shared/HTMLDOMPropertyConfig.js#L23-L25
     *
     * @param {String}
     * @return {Boolean}
     */
    isCustomAttribute: RegExp.prototype.test.bind(
      new RegExp('^(data|aria)-[' + ATTRIBUTE_NAME_CHAR + ']*$')
    )
  };

  var commonjsGlobal = typeof globalThis !== 'undefined' ? globalThis : typeof window !== 'undefined' ? window : typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : {};

  function createCommonjsModule(fn) {
    var module = { exports: {} };
  	return fn(module, module.exports), module.exports;
  }

  // http://www.w3.org/TR/CSS21/grammar.html
  // https://github.com/visionmedia/css-parse/pull/49#issuecomment-30088027
  var COMMENT_REGEX = /\/\*[^*]*\*+([^/*][^*]*\*+)*\//g;

  var NEWLINE_REGEX = /\n/g;
  var WHITESPACE_REGEX = /^\s*/;

  // declaration
  var PROPERTY_REGEX = /^(\*?[-#/*\\\w]+(\[[0-9a-z_-]+\])?)\s*/;
  var COLON_REGEX = /^:\s*/;
  var VALUE_REGEX = /^((?:'(?:\\'|.)*?'|"(?:\\"|.)*?"|\([^)]*?\)|[^};])+)/;
  var SEMICOLON_REGEX = /^[;\s]*/;

  // https://developer.mozilla.org/docs/Web/JavaScript/Reference/Global_Objects/String/Trim#Polyfill
  var TRIM_REGEX = /^\s+|\s+$/g;

  // strings
  var NEWLINE = '\n';
  var FORWARD_SLASH = '/';
  var ASTERISK = '*';
  var EMPTY_STRING = '';

  // types
  var TYPE_COMMENT = 'comment';
  var TYPE_DECLARATION = 'declaration';

  /**
   * @param {String} style
   * @param {Object} [options]
   * @return {Object[]}
   * @throws {TypeError}
   * @throws {Error}
   */
  var inlineStyleParser = function(style, options) {
    if (typeof style !== 'string') {
      throw new TypeError('First argument must be a string');
    }

    if (!style) return [];

    options = options || {};

    /**
     * Positional.
     */
    var lineno = 1;
    var column = 1;

    /**
     * Update lineno and column based on `str`.
     *
     * @param {String} str
     */
    function updatePosition(str) {
      var lines = str.match(NEWLINE_REGEX);
      if (lines) lineno += lines.length;
      var i = str.lastIndexOf(NEWLINE);
      column = ~i ? str.length - i : column + str.length;
    }

    /**
     * Mark position and patch `node.position`.
     *
     * @return {Function}
     */
    function position() {
      var start = { line: lineno, column: column };
      return function(node) {
        node.position = new Position(start);
        whitespace();
        return node;
      };
    }

    /**
     * Store position information for a node.
     *
     * @constructor
     * @property {Object} start
     * @property {Object} end
     * @property {undefined|String} source
     */
    function Position(start) {
      this.start = start;
      this.end = { line: lineno, column: column };
      this.source = options.source;
    }

    /**
     * Non-enumerable source string.
     */
    Position.prototype.content = style;

    /**
     * Error `msg`.
     *
     * @param {String} msg
     * @throws {Error}
     */
    function error(msg) {
      var err = new Error(
        options.source + ':' + lineno + ':' + column + ': ' + msg
      );
      err.reason = msg;
      err.filename = options.source;
      err.line = lineno;
      err.column = column;
      err.source = style;

      if (options.silent) ; else {
        throw err;
      }
    }

    /**
     * Match `re` and return captures.
     *
     * @param {RegExp} re
     * @return {undefined|Array}
     */
    function match(re) {
      var m = re.exec(style);
      if (!m) return;
      var str = m[0];
      updatePosition(str);
      style = style.slice(str.length);
      return m;
    }

    /**
     * Parse whitespace.
     */
    function whitespace() {
      match(WHITESPACE_REGEX);
    }

    /**
     * Parse comments.
     *
     * @param {Object[]} [rules]
     * @return {Object[]}
     */
    function comments(rules) {
      var c;
      rules = rules || [];
      while ((c = comment())) {
        if (c !== false) {
          rules.push(c);
        }
      }
      return rules;
    }

    /**
     * Parse comment.
     *
     * @return {Object}
     * @throws {Error}
     */
    function comment() {
      var pos = position();
      if (FORWARD_SLASH != style.charAt(0) || ASTERISK != style.charAt(1)) return;

      var i = 2;
      while (
        EMPTY_STRING != style.charAt(i) &&
        (ASTERISK != style.charAt(i) || FORWARD_SLASH != style.charAt(i + 1))
      ) {
        ++i;
      }
      i += 2;

      if (EMPTY_STRING === style.charAt(i - 1)) {
        return error('End of comment missing');
      }

      var str = style.slice(2, i - 2);
      column += 2;
      updatePosition(str);
      style = style.slice(i);
      column += 2;

      return pos({
        type: TYPE_COMMENT,
        comment: str
      });
    }

    /**
     * Parse declaration.
     *
     * @return {Object}
     * @throws {Error}
     */
    function declaration() {
      var pos = position();

      // prop
      var prop = match(PROPERTY_REGEX);
      if (!prop) return;
      comment();

      // :
      if (!match(COLON_REGEX)) return error("property missing ':'");

      // val
      var val = match(VALUE_REGEX);

      var ret = pos({
        type: TYPE_DECLARATION,
        property: trim(prop[0].replace(COMMENT_REGEX, EMPTY_STRING)),
        value: val
          ? trim(val[0].replace(COMMENT_REGEX, EMPTY_STRING))
          : EMPTY_STRING
      });

      // ;
      match(SEMICOLON_REGEX);

      return ret;
    }

    /**
     * Parse declarations.
     *
     * @return {Object[]}
     */
    function declarations() {
      var decls = [];

      comments(decls);

      // declarations
      var decl;
      while ((decl = declaration())) {
        if (decl !== false) {
          decls.push(decl);
          comments(decls);
        }
      }

      return decls;
    }

    whitespace();
    return declarations();
  };

  /**
   * Trim `str`.
   *
   * @param {String} str
   * @return {String}
   */
  function trim(str) {
    return str ? str.replace(TRIM_REGEX, EMPTY_STRING) : EMPTY_STRING;
  }

  /**
   * Parses inline style to object.
   *
   * @example
   * // returns { 'line-height': '42' }
   * StyleToObject('line-height: 42;');
   *
   * @param  {String}      style      - The inline style.
   * @param  {Function}    [iterator] - The iterator function.
   * @return {null|Object}
   */
  function StyleToObject(style, iterator) {
    var output = null;
    if (!style || typeof style !== 'string') {
      return output;
    }

    var declaration;
    var declarations = inlineStyleParser(style);
    var hasIterator = typeof iterator === 'function';
    var property;
    var value;

    for (var i = 0, len = declarations.length; i < len; i++) {
      declaration = declarations[i];
      property = declaration.property;
      value = declaration.value;

      if (hasIterator) {
        iterator(property, value, declaration);
      } else if (value) {
        output || (output = {});
        output[property] = value;
      }
    }

    return output;
  }

  var styleToObject = StyleToObject;

  var utilities = createCommonjsModule(function (module, exports) {
  exports.__esModule = true;
  exports.camelCase = void 0;
  var CUSTOM_PROPERTY_REGEX = /^--[a-zA-Z0-9-]+$/;
  var HYPHEN_REGEX = /-([a-z])/g;
  var NO_HYPHEN_REGEX = /^[^-]+$/;
  var VENDOR_PREFIX_REGEX = /^-(webkit|moz|ms|o|khtml)-/;
  var skipCamelCase = function (property) {
      return !property ||
          NO_HYPHEN_REGEX.test(property) ||
          CUSTOM_PROPERTY_REGEX.test(property);
  };
  var capitalize = function (match, character) {
      return character.toUpperCase();
  };
  var trimHyphen = function (match, prefix) { return prefix + "-"; };
  var camelCase = function (property, options) {
      if (options === void 0) { options = {}; }
      if (skipCamelCase(property)) {
          return property;
      }
      property = property.toLowerCase();
      if (!options.reactCompat) {
          property = property.replace(VENDOR_PREFIX_REGEX, trimHyphen);
      }
      return property.replace(HYPHEN_REGEX, capitalize);
  };
  exports.camelCase = camelCase;
  });

  var cjs = createCommonjsModule(function (module, exports) {
  var __importDefault = (commonjsGlobal && commonjsGlobal.__importDefault) || function (mod) {
      return (mod && mod.__esModule) ? mod : { "default": mod };
  };
  exports.__esModule = true;
  var style_to_object_1 = __importDefault(styleToObject);

  function StyleToJS(style, options) {
      var output = {};
      if (!style || typeof style !== 'string') {
          return output;
      }
      style_to_object_1["default"](style, function (property, value) {
          if (property && value) {
              output[utilities.camelCase(property, options)] = value;
          }
      });
      return output;
  }
  exports["default"] = StyleToJS;
  });

  var styleToJS = cjs.default;

  /**
   * Swap key with value in an object.
   *
   * @param  {Object}   obj        - The object.
   * @param  {Function} [override] - The override method.
   * @return {Object}              - The inverted object.
   */
  function invertObject(obj, override) {
    if (!obj || typeof obj !== 'object') {
      throw new TypeError('First argument must be an object');
    }

    var key;
    var value;
    var isOverridePresent = typeof override === 'function';
    var overrides = {};
    var result = {};

    for (key in obj) {
      value = obj[key];

      if (isOverridePresent) {
        overrides = override(key, value);
        if (overrides && overrides.length === 2) {
          result[overrides[0]] = overrides[1];
          continue;
        }
      }

      if (typeof value === 'string') {
        result[value] = key;
      }
    }

    return result;
  }

  /**
   * Check if a given tag is a custom component.
   *
   * @see {@link https://github.com/facebook/react/blob/v16.6.3/packages/react-dom/src/shared/isCustomComponent.js}
   *
   * @param {string} tagName - The name of the html tag.
   * @param {Object} props   - The props being passed to the element.
   * @return {boolean}
   */
  function isCustomComponent(tagName, props) {
    if (tagName.indexOf('-') === -1) {
      return props && typeof props.is === 'string';
    }

    switch (tagName) {
      // These are reserved SVG and MathML elements.
      // We don't mind this whitelist too much because we expect it to never grow.
      // The alternative is to track the namespace in a few places which is convoluted.
      // https://w3c.github.io/webcomponents/spec/custom/#custom-elements-core-concepts
      case 'annotation-xml':
      case 'color-profile':
      case 'font-face':
      case 'font-face-src':
      case 'font-face-uri':
      case 'font-face-format':
      case 'font-face-name':
      case 'missing-glyph':
        return false;
      default:
        return true;
    }
  }

  var styleToJSOptions = { reactCompat: true };

  /**
   * Sets style prop.
   *
   * @param {null|undefined|string} style
   * @param {object} props
   */
  function setStyleProp(style, props) {
    if (style === null || style === undefined) {
      return;
    }
    props.style = styleToJS(style, styleToJSOptions);
  }

  /**
   * @constant {boolean}
   * @see {@link https://reactjs.org/blog/2017/09/08/dom-attributes-in-react-16.html}
   */
  var PRESERVE_CUSTOM_ATTRIBUTES = React__default['default'].version.split('.')[0] >= 16;

  var utilities$1 = {
    PRESERVE_CUSTOM_ATTRIBUTES: PRESERVE_CUSTOM_ATTRIBUTES,
    invertObject: invertObject,
    isCustomComponent: isCustomComponent,
    setStyleProp: setStyleProp
  };

  var setStyleProp$1 = utilities$1.setStyleProp;

  var htmlProperties = reactProperty.html;
  var svgProperties = reactProperty.svg;
  var isCustomAttribute = reactProperty.isCustomAttribute;

  var hasOwnProperty = Object.prototype.hasOwnProperty;

  /**
   * Converts HTML/SVG DOM attributes to React props.
   *
   * @param  {object} [attributes={}] - HTML/SVG DOM attributes.
   * @return {object}                 - React props.
   */
  function attributesToProps(attributes) {
    attributes = attributes || {};

    var attributeName;
    var attributeNameLowerCased;
    var attributeValue;
    var property;
    var props = {};

    for (attributeName in attributes) {
      attributeValue = attributes[attributeName];

      // ARIA (aria-*) or custom data (data-*) attribute
      if (isCustomAttribute(attributeName)) {
        props[attributeName] = attributeValue;
        continue;
      }

      // convert HTML attribute to React prop
      attributeNameLowerCased = attributeName.toLowerCase();
      if (hasOwnProperty.call(htmlProperties, attributeNameLowerCased)) {
        property = htmlProperties[attributeNameLowerCased];
        props[property.propertyName] =
          property.hasBooleanValue ||
          (property.hasOverloadedBooleanValue && !attributeValue)
            ? true
            : attributeValue;
        continue;
      }

      // convert SVG attribute to React prop
      if (hasOwnProperty.call(svgProperties, attributeName)) {
        property = svgProperties[attributeName];
        props[property.propertyName] = attributeValue;
        continue;
      }

      // preserve custom attribute if React >=16
      if (utilities$1.PRESERVE_CUSTOM_ATTRIBUTES) {
        props[attributeName] = attributeValue;
      }
    }

    // transform inline style to object
    setStyleProp$1(attributes.style, props);

    return props;
  }

  var attributesToProps_1 = attributesToProps;

  var setStyleProp$2 = utilities$1.setStyleProp;

  /**
   * Converts DOM nodes to JSX element(s).
   *
   * @param  {DomElement[]} nodes             - DOM nodes.
   * @param  {object}       [options={}]      - Options.
   * @param  {Function}     [options.replace] - Replacer.
   * @param  {object}       [options.library] - Library (React/Preact/etc.).
   * @return {string|JSX.Element|JSX.Element[]}
   */
  function domToReact(nodes, options) {
    options = options || {};

    var library = options.library || React__default['default'];
    var cloneElement = library.cloneElement;
    var createElement = library.createElement;
    var isValidElement = library.isValidElement;

    var result = [];
    var node;
    var hasReplace = typeof options.replace === 'function';
    var replaceElement;
    var props;
    var children;
    var data;
    var trim = options.trim;

    for (var i = 0, len = nodes.length; i < len; i++) {
      node = nodes[i];

      // replace with custom React element (if present)
      if (hasReplace) {
        replaceElement = options.replace(node);

        if (isValidElement(replaceElement)) {
          // set "key" prop for sibling elements
          // https://fb.me/react-warning-keys
          if (len > 1) {
            replaceElement = cloneElement(replaceElement, {
              key: replaceElement.key || i
            });
          }
          result.push(replaceElement);
          continue;
        }
      }

      if (node.type === 'text') {
        // if trim option is enabled, skip whitespace text nodes
        if (trim) {
          data = node.data.trim();
          if (data) {
            result.push(node.data);
          }
        } else {
          result.push(node.data);
        }
        continue;
      }

      props = node.attribs;
      if (skipAttributesToProps(node)) {
        setStyleProp$2(props.style, props);
      } else if (props) {
        props = attributesToProps_1(props);
      }

      children = null;

      switch (node.type) {
        case 'script':
        case 'style':
          // prevent text in <script> or <style> from being escaped
          // https://reactjs.org/docs/dom-elements.html#dangerouslysetinnerhtml
          if (node.children[0]) {
            props.dangerouslySetInnerHTML = {
              __html: node.children[0].data
            };
          }
          break;

        case 'tag':
          // setting textarea value in children is an antipattern in React
          // https://reactjs.org/docs/forms.html#the-textarea-tag
          if (node.name === 'textarea' && node.children[0]) {
            props.defaultValue = node.children[0].data;
          } else if (node.children && node.children.length) {
            // continue recursion of creating React elements (if applicable)
            children = domToReact(node.children, options);
          }
          break;

        // skip all other cases (e.g., comment)
        default:
          continue;
      }

      // set "key" prop for sibling elements
      // https://fb.me/react-warning-keys
      if (len > 1) {
        props.key = i;
      }

      result.push(createElement(node.name, props, children));
    }

    return result.length === 1 ? result[0] : result;
  }

  /**
   * Determines whether DOM element attributes should be transformed to props.
   * Web Components should not have their attributes transformed except for `style`.
   *
   * @param  {DomElement} node
   * @return {boolean}
   */
  function skipAttributesToProps(node) {
    return (
      utilities$1.PRESERVE_CUSTOM_ATTRIBUTES &&
      node.type === 'tag' &&
      utilities$1.isCustomComponent(node.name, node.attribs)
    );
  }

  var domToReact_1 = domToReact;

  /**
   * SVG elements are case-sensitive.
   *
   * @see {@link https://developer.mozilla.org/docs/Web/SVG/Element#SVG_elements_A_to_Z}
   */
  var CASE_SENSITIVE_TAG_NAMES = [
    'animateMotion',
    'animateTransform',
    'clipPath',
    'feBlend',
    'feColorMatrix',
    'feComponentTransfer',
    'feComposite',
    'feConvolveMatrix',
    'feDiffuseLighting',
    'feDisplacementMap',
    'feDropShadow',
    'feFlood',
    'feFuncA',
    'feFuncB',
    'feFuncG',
    'feFuncR',
    'feGaussainBlur',
    'feImage',
    'feMerge',
    'feMergeNode',
    'feMorphology',
    'feOffset',
    'fePointLight',
    'feSpecularLighting',
    'feSpotLight',
    'feTile',
    'feTurbulence',
    'foreignObject',
    'linearGradient',
    'radialGradient',
    'textPath'
  ];

  var constants = {
    CASE_SENSITIVE_TAG_NAMES: CASE_SENSITIVE_TAG_NAMES
  };

  var node = createCommonjsModule(function (module, exports) {
  var __extends = (commonjsGlobal && commonjsGlobal.__extends) || (function () {
      var extendStatics = function (d, b) {
          extendStatics = Object.setPrototypeOf ||
              ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
              function (d, b) { for (var p in b) if (Object.prototype.hasOwnProperty.call(b, p)) d[p] = b[p]; };
          return extendStatics(d, b);
      };
      return function (d, b) {
          extendStatics(d, b);
          function __() { this.constructor = d; }
          d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
      };
  })();
  var __assign = (commonjsGlobal && commonjsGlobal.__assign) || function () {
      __assign = Object.assign || function(t) {
          for (var s, i = 1, n = arguments.length; i < n; i++) {
              s = arguments[i];
              for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p))
                  t[p] = s[p];
          }
          return t;
      };
      return __assign.apply(this, arguments);
  };
  Object.defineProperty(exports, "__esModule", { value: true });
  exports.cloneNode = exports.Element = exports.Document = exports.NodeWithChildren = exports.ProcessingInstruction = exports.Comment = exports.Text = exports.DataNode = exports.Node = void 0;
  var nodeTypes = new Map([
      ["tag" /* Tag */, 1],
      ["script" /* Script */, 1],
      ["style" /* Style */, 1],
      ["directive" /* Directive */, 1],
      ["text" /* Text */, 3],
      ["cdata" /* CDATA */, 4],
      ["comment" /* Comment */, 8],
      ["root" /* Root */, 9],
  ]);
  /**
   * This object will be used as the prototype for Nodes when creating a
   * DOM-Level-1-compliant structure.
   */
  var Node = /** @class */ (function () {
      /**
       *
       * @param type The type of the node.
       */
      function Node(type) {
          this.type = type;
          /** Parent of the node */
          this.parent = null;
          /** Previous sibling */
          this.prev = null;
          /** Next sibling */
          this.next = null;
          /** The start index of the node. Requires `withStartIndices` on the handler to be `true. */
          this.startIndex = null;
          /** The end index of the node. Requires `withEndIndices` on the handler to be `true. */
          this.endIndex = null;
      }
      Object.defineProperty(Node.prototype, "nodeType", {
          // Read-only aliases
          get: function () {
              var _a;
              return (_a = nodeTypes.get(this.type)) !== null && _a !== void 0 ? _a : 1;
          },
          enumerable: false,
          configurable: true
      });
      Object.defineProperty(Node.prototype, "parentNode", {
          // Read-write aliases for properties
          get: function () {
              return this.parent;
          },
          set: function (parent) {
              this.parent = parent;
          },
          enumerable: false,
          configurable: true
      });
      Object.defineProperty(Node.prototype, "previousSibling", {
          get: function () {
              return this.prev;
          },
          set: function (prev) {
              this.prev = prev;
          },
          enumerable: false,
          configurable: true
      });
      Object.defineProperty(Node.prototype, "nextSibling", {
          get: function () {
              return this.next;
          },
          set: function (next) {
              this.next = next;
          },
          enumerable: false,
          configurable: true
      });
      /**
       * Clone this node, and optionally its children.
       *
       * @param recursive Clone child nodes as well.
       * @returns A clone of the node.
       */
      Node.prototype.cloneNode = function (recursive) {
          if (recursive === void 0) { recursive = false; }
          return cloneNode(this, recursive);
      };
      return Node;
  }());
  exports.Node = Node;
  var DataNode = /** @class */ (function (_super) {
      __extends(DataNode, _super);
      /**
       * @param type The type of the node
       * @param data The content of the data node
       */
      function DataNode(type, data) {
          var _this = _super.call(this, type) || this;
          _this.data = data;
          return _this;
      }
      Object.defineProperty(DataNode.prototype, "nodeValue", {
          get: function () {
              return this.data;
          },
          set: function (data) {
              this.data = data;
          },
          enumerable: false,
          configurable: true
      });
      return DataNode;
  }(Node));
  exports.DataNode = DataNode;
  var Text = /** @class */ (function (_super) {
      __extends(Text, _super);
      function Text(data) {
          return _super.call(this, "text" /* Text */, data) || this;
      }
      return Text;
  }(DataNode));
  exports.Text = Text;
  var Comment = /** @class */ (function (_super) {
      __extends(Comment, _super);
      function Comment(data) {
          return _super.call(this, "comment" /* Comment */, data) || this;
      }
      return Comment;
  }(DataNode));
  exports.Comment = Comment;
  var ProcessingInstruction = /** @class */ (function (_super) {
      __extends(ProcessingInstruction, _super);
      function ProcessingInstruction(name, data) {
          var _this = _super.call(this, "directive" /* Directive */, data) || this;
          _this.name = name;
          return _this;
      }
      return ProcessingInstruction;
  }(DataNode));
  exports.ProcessingInstruction = ProcessingInstruction;
  /**
   * A `Node` that can have children.
   */
  var NodeWithChildren = /** @class */ (function (_super) {
      __extends(NodeWithChildren, _super);
      /**
       * @param type Type of the node.
       * @param children Children of the node. Only certain node types can have children.
       */
      function NodeWithChildren(type, children) {
          var _this = _super.call(this, type) || this;
          _this.children = children;
          return _this;
      }
      Object.defineProperty(NodeWithChildren.prototype, "firstChild", {
          // Aliases
          get: function () {
              var _a;
              return (_a = this.children[0]) !== null && _a !== void 0 ? _a : null;
          },
          enumerable: false,
          configurable: true
      });
      Object.defineProperty(NodeWithChildren.prototype, "lastChild", {
          get: function () {
              return this.children.length > 0
                  ? this.children[this.children.length - 1]
                  : null;
          },
          enumerable: false,
          configurable: true
      });
      Object.defineProperty(NodeWithChildren.prototype, "childNodes", {
          get: function () {
              return this.children;
          },
          set: function (children) {
              this.children = children;
          },
          enumerable: false,
          configurable: true
      });
      return NodeWithChildren;
  }(Node));
  exports.NodeWithChildren = NodeWithChildren;
  var Document = /** @class */ (function (_super) {
      __extends(Document, _super);
      function Document(children) {
          return _super.call(this, "root" /* Root */, children) || this;
      }
      return Document;
  }(NodeWithChildren));
  exports.Document = Document;
  var Element = /** @class */ (function (_super) {
      __extends(Element, _super);
      /**
       * @param name Name of the tag, eg. `div`, `span`.
       * @param attribs Object mapping attribute names to attribute values.
       * @param children Children of the node.
       */
      function Element(name, attribs, children) {
          if (children === void 0) { children = []; }
          var _this = _super.call(this, name === "script"
              ? "script" /* Script */
              : name === "style"
                  ? "style" /* Style */
                  : "tag" /* Tag */, children) || this;
          _this.name = name;
          _this.attribs = attribs;
          _this.attribs = attribs;
          return _this;
      }
      Object.defineProperty(Element.prototype, "tagName", {
          // DOM Level 1 aliases
          get: function () {
              return this.name;
          },
          set: function (name) {
              this.name = name;
          },
          enumerable: false,
          configurable: true
      });
      Object.defineProperty(Element.prototype, "attributes", {
          get: function () {
              var _this = this;
              return Object.keys(this.attribs).map(function (name) {
                  var _a, _b;
                  return ({
                      name: name,
                      value: _this.attribs[name],
                      namespace: (_a = _this["x-attribsNamespace"]) === null || _a === void 0 ? void 0 : _a[name],
                      prefix: (_b = _this["x-attribsPrefix"]) === null || _b === void 0 ? void 0 : _b[name],
                  });
              });
          },
          enumerable: false,
          configurable: true
      });
      return Element;
  }(NodeWithChildren));
  exports.Element = Element;
  /**
   * Clone a node, and optionally its children.
   *
   * @param recursive Clone child nodes as well.
   * @returns A clone of the node.
   */
  function cloneNode(node, recursive) {
      if (recursive === void 0) { recursive = false; }
      var result;
      switch (node.type) {
          case "text" /* Text */:
              result = new Text(node.data);
              break;
          case "directive" /* Directive */: {
              var instr = node;
              result = new ProcessingInstruction(instr.name, instr.data);
              if (instr["x-name"] != null) {
                  result["x-name"] = instr["x-name"];
                  result["x-publicId"] = instr["x-publicId"];
                  result["x-systemId"] = instr["x-systemId"];
              }
              break;
          }
          case "comment" /* Comment */:
              result = new Comment(node.data);
              break;
          case "tag" /* Tag */:
          case "script" /* Script */:
          case "style" /* Style */: {
              var elem = node;
              var children = recursive ? cloneChildren(elem.children) : [];
              var clone_1 = new Element(elem.name, __assign({}, elem.attribs), children);
              children.forEach(function (child) { return (child.parent = clone_1); });
              if (elem["x-attribsNamespace"]) {
                  clone_1["x-attribsNamespace"] = __assign({}, elem["x-attribsNamespace"]);
              }
              if (elem["x-attribsPrefix"]) {
                  clone_1["x-attribsPrefix"] = __assign({}, elem["x-attribsPrefix"]);
              }
              result = clone_1;
              break;
          }
          case "cdata" /* CDATA */: {
              var cdata = node;
              var children = recursive ? cloneChildren(cdata.children) : [];
              var clone_2 = new NodeWithChildren(node.type, children);
              children.forEach(function (child) { return (child.parent = clone_2); });
              result = clone_2;
              break;
          }
          case "root" /* Root */: {
              var doc = node;
              var children = recursive ? cloneChildren(doc.children) : [];
              var clone_3 = new Document(children);
              children.forEach(function (child) { return (child.parent = clone_3); });
              if (doc["x-mode"]) {
                  clone_3["x-mode"] = doc["x-mode"];
              }
              result = clone_3;
              break;
          }
          case "doctype" /* Doctype */: {
              // This type isn't used yet.
              throw new Error("Not implemented yet: ElementType.Doctype case");
          }
      }
      result.startIndex = node.startIndex;
      result.endIndex = node.endIndex;
      return result;
  }
  exports.cloneNode = cloneNode;
  function cloneChildren(childs) {
      var children = childs.map(function (child) { return cloneNode(child, true); });
      for (var i = 1; i < children.length; i++) {
          children[i].prev = children[i - 1];
          children[i - 1].next = children[i];
      }
      return children;
  }
  });

  var CASE_SENSITIVE_TAG_NAMES$1 = constants.CASE_SENSITIVE_TAG_NAMES;

  var Comment = node.Comment;
  var Element = node.Element;
  var ProcessingInstruction = node.ProcessingInstruction;
  var Text = node.Text;

  var caseSensitiveTagNamesMap = {};
  var tagName;

  for (var i = 0, len = CASE_SENSITIVE_TAG_NAMES$1.length; i < len; i++) {
    tagName = CASE_SENSITIVE_TAG_NAMES$1[i];
    caseSensitiveTagNamesMap[tagName.toLowerCase()] = tagName;
  }

  /**
   * Gets case-sensitive tag name.
   *
   * @param  {string}           tagName - Tag name in lowercase.
   * @return {string|undefined}         - Case-sensitive tag name.
   */
  function getCaseSensitiveTagName(tagName) {
    return caseSensitiveTagNamesMap[tagName];
  }

  /**
   * Formats DOM attributes to a hash map.
   *
   * @param  {NamedNodeMap} attributes - List of attributes.
   * @return {object}                  - Map of attribute name to value.
   */
  function formatAttributes(attributes) {
    var result = {};
    var attribute;
    // `NamedNodeMap` is array-like
    for (var i = 0, len = attributes.length; i < len; i++) {
      attribute = attributes[i];
      result[attribute.name] = attribute.value;
    }
    return result;
  }

  /**
   * Corrects the tag name if it is case-sensitive (SVG).
   * Otherwise, returns the lowercase tag name (HTML).
   *
   * @param  {string} tagName - Lowercase tag name.
   * @return {string}         - Formatted tag name.
   */
  function formatTagName(tagName) {
    tagName = tagName.toLowerCase();
    var caseSensitiveTagName = getCaseSensitiveTagName(tagName);
    if (caseSensitiveTagName) {
      return caseSensitiveTagName;
    }
    return tagName;
  }

  /**
   * Transforms DOM nodes to `domhandler` nodes.
   *
   * @param  {NodeList}     nodes         - DOM nodes.
   * @param  {Element|null} [parent=null] - Parent node.
   * @param  {string}       [directive]   - Directive.
   * @return {Array<Comment|Element|ProcessingInstruction|Text>}
   */
  function formatDOM(nodes, parent, directive) {
    parent = parent || null;
    var result = [];

    for (var index = 0, len = nodes.length; index < len; index++) {
      var node = nodes[index];
      var current;

      // set the node data given the type
      switch (node.nodeType) {
        case 1:
          // script, style, or tag
          current = new Element(
            formatTagName(node.nodeName),
            formatAttributes(node.attributes)
          );
          current.children = formatDOM(node.childNodes, current);
          break;

        case 3:
          current = new Text(node.nodeValue);
          break;

        case 8:
          current = new Comment(node.nodeValue);
          break;

        default:
          continue;
      }

      // set previous node next
      var prev = result[index - 1] || null;
      if (prev) {
        prev.next = current;
      }

      // set properties for current node
      current.parent = parent;
      current.prev = prev;
      current.next = null;

      result.push(current);
    }

    if (directive) {
      current = new ProcessingInstruction(
        directive.substring(0, directive.indexOf(' ')).toLowerCase(),
        directive
      );
      current.next = result[0] || null;
      current.parent = parent;
      result.unshift(current);

      if (result[1]) {
        result[1].prev = result[0];
      }
    }

    return result;
  }

  /**
   * Detects if browser is Internet Explorer.
   *
   * @return {boolean} - Whether IE is detected.
   */
  function isIE() {
    return /(MSIE |Trident\/|Edge\/)/.test(navigator.userAgent);
  }

  var utilities$2 = {
    formatAttributes: formatAttributes,
    formatDOM: formatDOM,
    isIE: isIE
  };

  // constants
  var HTML = 'html';
  var HEAD = 'head';
  var BODY = 'body';
  var FIRST_TAG_REGEX = /<([a-zA-Z]+[0-9]?)/; // e.g., <h1>
  var HEAD_TAG_REGEX = /<head.*>/i;
  var BODY_TAG_REGEX = /<body.*>/i;

  // falls back to `parseFromString` if `createHTMLDocument` cannot be used
  var parseFromDocument = function () {
    throw new Error(
      'This browser does not support `document.implementation.createHTMLDocument`'
    );
  };

  var parseFromString = function () {
    throw new Error(
      'This browser does not support `DOMParser.prototype.parseFromString`'
    );
  };

  /**
   * DOMParser (performance: slow).
   *
   * @see https://developer.mozilla.org/docs/Web/API/DOMParser#Parsing_an_SVG_or_HTML_document
   */
  if (typeof window.DOMParser === 'function') {
    var domParser = new window.DOMParser();
    var mimeType = 'text/html';

    /**
     * Creates an HTML document using `DOMParser.parseFromString`.
     *
     * @param  {string} html      - The HTML string.
     * @param  {string} [tagName] - The element to render the HTML (with 'body' as fallback).
     * @return {HTMLDocument}
     */
    parseFromString = function (html, tagName) {
      if (tagName) {
        html = '<' + tagName + '>' + html + '</' + tagName + '>';
      }

      return domParser.parseFromString(html, mimeType);
    };

    parseFromDocument = parseFromString;
  }

  /**
   * DOMImplementation (performance: fair).
   *
   * @see https://developer.mozilla.org/docs/Web/API/DOMImplementation/createHTMLDocument
   */
  if (document.implementation) {
    var isIE$1 = utilities$2.isIE;

    // title parameter is required in IE
    // https://msdn.microsoft.com/en-us/library/ff975457(v=vs.85).aspx
    var doc = document.implementation.createHTMLDocument(
      isIE$1() ? 'html-dom-parser' : undefined
    );

    /**
     * Use HTML document created by `document.implementation.createHTMLDocument`.
     *
     * @param  {string} html      - The HTML string.
     * @param  {string} [tagName] - The element to render the HTML (with 'body' as fallback).
     * @return {HTMLDocument}
     */
    parseFromDocument = function (html, tagName) {
      if (tagName) {
        doc.documentElement.getElementsByTagName(tagName)[0].innerHTML = html;
        return doc;
      }

      doc.documentElement.innerHTML = html;
      return doc;
    };
  }

  /**
   * Template (performance: fast).
   *
   * @see https://developer.mozilla.org/docs/Web/HTML/Element/template
   */
  var template = document.createElement('template');
  var parseFromTemplate;

  if (template.content) {
    /**
     * Uses a template element (content fragment) to parse HTML.
     *
     * @param  {string} html - The HTML string.
     * @return {NodeList}
     */
    parseFromTemplate = function (html) {
      template.innerHTML = html;
      return template.content.childNodes;
    };
  }

  /**
   * Parses HTML string to DOM nodes.
   *
   * @param  {string}   html - HTML markup.
   * @return {NodeList}
   */
  function domparser(html) {
    var firstTagName;
    var match = html.match(FIRST_TAG_REGEX);

    if (match && match[1]) {
      firstTagName = match[1].toLowerCase();
    }

    var doc;
    var element;
    var elements;

    switch (firstTagName) {
      case HTML:
        doc = parseFromString(html);

        // the created document may come with filler head/body elements,
        // so make sure to remove them if they don't actually exist
        if (!HEAD_TAG_REGEX.test(html)) {
          element = doc.getElementsByTagName(HEAD)[0];
          if (element) {
            element.parentNode.removeChild(element);
          }
        }

        if (!BODY_TAG_REGEX.test(html)) {
          element = doc.getElementsByTagName(BODY)[0];
          if (element) {
            element.parentNode.removeChild(element);
          }
        }

        return doc.getElementsByTagName(HTML);

      case HEAD:
      case BODY:
        elements = parseFromDocument(html).getElementsByTagName(firstTagName);

        // if there's a sibling element, then return both elements
        if (BODY_TAG_REGEX.test(html) && HEAD_TAG_REGEX.test(html)) {
          return elements[0].parentNode.childNodes;
        }
        return elements;

      // low-level tag or text
      default:
        if (parseFromTemplate) {
          return parseFromTemplate(html);
        }

        return parseFromDocument(html, BODY).getElementsByTagName(BODY)[0]
          .childNodes;
    }
  }

  var domparser_1 = domparser;

  var formatDOM$1 = utilities$2.formatDOM;

  var DIRECTIVE_REGEX = /<(![a-zA-Z\s]+)>/; // e.g., <!doctype html>

  /**
   * Parses HTML string to DOM nodes in browser.
   *
   * @param  {string} html  - HTML markup.
   * @return {DomElement[]} - DOM elements.
   */
  function HTMLDOMParser(html) {
    if (typeof html !== 'string') {
      throw new TypeError('First argument must be a string');
    }

    if (html === '') {
      return [];
    }

    // match directive
    var match = html.match(DIRECTIVE_REGEX);
    var directive;

    if (match && match[1]) {
      directive = match[1];
    }

    return formatDOM$1(domparser_1(html), null, directive);
  }

  var htmlToDom = HTMLDOMParser;

  var domParserOptions = { lowerCaseAttributeNames: false };

  /**
   * Converts HTML string to React elements.
   *
   * @param  {String}   html                    - HTML string.
   * @param  {Object}   [options]               - Parser options.
   * @param  {Object}   [options.htmlparser2]   - htmlparser2 options.
   * @param  {Object}   [options.library]       - Library for React, Preact, etc.
   * @param  {Function} [options.replace]       - Replace method.
   * @return {JSX.Element|JSX.Element[]|String} - React element(s), empty array, or string.
   */
  function HTMLReactParser(html, options) {
    if (typeof html !== 'string') {
      throw new TypeError('First argument must be a string');
    }
    if (html === '') {
      return [];
    }
    options = options || {};
    return domToReact_1(
      htmlToDom(html, options.htmlparser2 || domParserOptions),
      options
    );
  }

  HTMLReactParser.domToReact = domToReact_1;
  HTMLReactParser.htmlToDOM = htmlToDom;
  HTMLReactParser.attributesToProps = attributesToProps_1;

  // support CommonJS and ES Modules
  var htmlReactParser = HTMLReactParser;
  var _default = HTMLReactParser;
  htmlReactParser.default = _default;

  return htmlReactParser;

})));
//# sourceMappingURL=html-react-parser.js.map

export const dependencies = {
  'a11y/semantic-enrich': ['input/mml', '[sre]'],
  'a11y/complexity': ['a11y/semantic-enrich'],
  'a11y/explorer': ['a11y/semantic-enrich', 'ui/menu'],
  '[tex]/all-packages': ['input/tex-base'],
  '[tex]/action': ['input/tex-base', '[tex]/newcommand'],
  '[tex]/autoload': ['input/tex-base', '[tex]/require'],
  '[tex]/ams': ['input/tex-base'],
  '[tex]/amscd': ['input/tex-base'],
  '[tex]/bbox': ['input/tex-base', '[tex]/ams', '[tex]/newcommand'],
  '[tex]/boldsymbol': ['input/tex-base'],
  '[tex]/braket': ['input/tex-base'],
  '[tex]/bussproofs': ['input/tex-base'],
  '[tex]/cancel': ['input/tex-base', '[tex]/enclose'],
  '[tex]/color': ['input/tex-base'],
  '[tex]/colorv2': ['input/tex-base'],
  '[tex]/configmacros': ['input/tex-base', '[tex]/newcommand'],
  '[tex]/enclose': ['input/tex-base'],
  '[tex]/extpfeil': ['input/tex-base', '[tex]/newcommand', '[tex]/ams'],
  '[tex]/html': ['input/tex-base'],
  '[tex]/mhchem': ['input/tex-base', '[tex]/ams'],
  '[tex]/newcommand': ['input/tex-base'],
  '[tex]/noerrors': ['input/tex-base'],
  '[tex]/noundefined': ['input/tex-base'],
  '[tex]/physics': ['input/tex-base'],
  '[tex]/require': ['input/tex-base'],
  '[tex]/tagformat': ['input/tex-base'],
  '[tex]/textmacros': ['input/tex-base'],
  '[tex]/unicode': ['input/tex-base'],
  '[tex]/verb': ['input/tex-base']
};

export const paths = {
  tex: '[mathjax]/input/tex/extensions',
  sre: '[mathjax]/sre/' + (typeof window === 'undefined' ? 'sre-node' : 'sre_browser')
};

const allPackages = [
  '[tex]/action',
  '[tex]/ams',
  '[tex]/amscd',
  '[tex]/bbox',
  '[tex]/boldsymbol',
  '[tex]/braket',
  '[tex]/bussproofs',
  '[tex]/cancel',
  '[tex]/color',
  '[tex]/configmacros',
  '[tex]/enclose',
  '[tex]/extpfeil',
  '[tex]/html',
  '[tex]/mhchem',
  '[tex]/newcommand',
  '[tex]/noerrors',
  '[tex]/noundefined',
  '[tex]/physics',
  '[tex]/require',
  '[tex]/tagformat',
  '[tex]/textmacros',
  '[tex]/unicode',
  '[tex]/verb'
];

export const provides = {
  'startup': ['loader'],
  'input/tex': [
    'input/tex-base',
    '[tex]/ams',
    '[tex]/newcommand',
    '[tex]/noundefined',
    '[tex]/require',
    '[tex]/autoload',
    '[tex]/configmacros'
  ],
  'input/tex-full': [
    'input/tex-base',
    '[tex]/all-packages',
    ...allPackages
  ],
  '[tex]/all-packages': allPackages
};

//
//  Compatibility with v3.0 names for TeX extensions
//
export const compatibility = {
  '[tex]/amsCd': '[tex]/amscd',
  '[tex]/colorV2': '[tex]/colorv2',
  '[tex]/configMacros': '[tex]/configmacros',
  '[tex]/tagFormat': '[tex]/tagformat'
};

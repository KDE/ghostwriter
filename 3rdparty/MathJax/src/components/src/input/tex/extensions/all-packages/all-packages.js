import './lib/all-packages.js';

import {AllPackages} from '../../../../../../js/input/tex/AllPackages.js';
import '../../../../../../js/input/tex/autoload/AutoloadConfiguration.js';
import '../../../../../../js/input/tex/require/RequireConfiguration.js';
import {insert} from '../../../../../../js/util/Options.js';

if (MathJax.loader) {
  MathJax.loader.preLoad('[tex]/autoload', '[tex]/require');
}
if (MathJax.startup) {
  if (!MathJax.config.tex) {
    MathJax.config.tex = {};
  }
  let packages = MathJax.config.tex.packages;
  MathJax.config.tex.packages = ['autoload', 'require', ...AllPackages];
  if (packages) {
    insert(MathJax.config.tex, {packages});
  }
}

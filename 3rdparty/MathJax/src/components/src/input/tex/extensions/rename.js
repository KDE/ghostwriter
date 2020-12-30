import {combineConfig} from '../../../../../js/components/global.js';

//
// Look for a package name in the package list and change it to a new name
//   and rename tex options for it, if there are any.
//
export function rename(oname, nname, options) {
  const tex = MathJax.config.tex;
  if (tex && tex.packages) {
    const packages = tex.packages;
    const n = packages.indexOf(oname);
    if (n >= 0) packages[n] = nname;
    if (options && tex[oname]) {
      combineConfig(tex, {[nname]: tex[oname]});
      delete tex[oname];
    }
  }
}

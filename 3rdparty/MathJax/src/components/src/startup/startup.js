import './lib/startup.js';

import {Loader, CONFIG} from '../../../js/components/loader.js';
import {combineDefaults} from '../../../js/components/global.js';
import {dependencies, paths, provides, compatibility} from '../dependencies.js';

combineDefaults(MathJax.config.loader, 'dependencies', dependencies);
combineDefaults(MathJax.config.loader, 'paths', paths);
combineDefaults(MathJax.config.loader, 'provides', provides);
combineDefaults(MathJax.config.loader, 'source', compatibility);

Loader.preLoad('loader');

Loader.load(...CONFIG.load)
      .then(() => CONFIG.ready())
      .catch(error => CONFIG.failed(error));

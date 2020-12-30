import './lib/tex-full.js';

import {registerTeX} from '../tex/register.js';
import {rename} from '../tex/extensions/rename.js';
import {Loader} from '../../../../js/components/loader.js';
import {AllPackages} from '../../../../js/input/tex/AllPackages.js';
import '../../../../js/input/tex/require/RequireConfiguration.js';

Loader.preLoad(
  'input/tex-base',
  '[tex]/all-packages',
  '[tex]/require'
);

registerTeX(['require',...AllPackages]);
rename('amsCd', 'amscd', true);
rename('colorV2', 'colorv2', false);
rename('configMacros', 'configmacros', false);
rename('tagFormat', 'tagformat', true);

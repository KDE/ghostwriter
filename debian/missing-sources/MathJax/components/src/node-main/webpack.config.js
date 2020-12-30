const PACKAGE = require('../../webpack.common.js');

const package = PACKAGE(
  'node-main',                        // the package to build
  '../../../js',                      // location of the MathJax js library
  [],                                 // packages to link to
  __dirname                           // our directory
);

package.output.libraryTarget = 'this';  // make node-main.js exports available to caller

module.exports = package;

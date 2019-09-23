import './styles/style.scss';
import 'impress.js';

import renderMathInElement from 'katex/dist/contrib/auto-render';
import 'katex/dist/katex.css';

import {stepEnterListener as moreStepEnterListener} from "./modules/showMore/showMore";

/* Key mapping:
 *  A - Toggles "more" content
 *  Excape - Exits from "more"
 *  S - Toggles video playback on last slide
 */

// Cause switching slides while Alt-Tabbing around is driving me mad
document.addEventListener('keyup', function (e) {
  if (e.key === 'Tab') e.stopPropagation();
}, true);

document.addEventListener('DOMContentLoaded', function () {
  renderMathInElement(document.body, {
    delimiters: [
      {left: "$$", right: "$$", display: true},
      {left: "\\(", right: "\\)", display: false},
      {left: "\\[", right: "\\]", display: true}
    ]
  });
});

const impressApi = impress();
const impressRoot = document.getElementById('impress');

const handler = function (e) {
  if (e.type === 'impress:stepleave') {
    console.log('Leaving step "' + e.target.id + '" (target hint: "' + e.detail.next.id + '")');
  } else if (e.type === 'impress:stepenter') {
    console.log('Entered step "' + e.target.id + '"');
    moreStepEnterListener(e);
  }
};

impressApi.lib.gc.addEventListener(impressRoot, 'impress:stepenter', handler);
impressApi.lib.gc.addEventListener(impressRoot, 'impress:stepleave', handler);
impressApi.init();

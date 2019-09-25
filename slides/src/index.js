import './styles/style.scss';
import 'impress.js';

import renderMathInElement from 'katex/dist/contrib/auto-render';
import 'katex/dist/katex.css';

// Asciinema Player and its dependencies (ehm... React) are HUGE,
// load in a different chunk (and reconsider using it next time)
import(/* webpackChunkName: "asciinema" */ 'asciinema-player');
import 'asciinema-player/resources/public/css/asciinema-player.css'

import {stepEnterListener as moreStepEnterListener} from "./modules/showMore/showMore";

/* Key mapping:
 *  A - Toggles "more" content
 *  Excape - Exits from "more"
 *  S - Toggles video playback on last slide
 */

let video = null;

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
  video = document.querySelector('#last-slide > asciinema-player');
});

const impressApi = impress();
const impressRoot = document.getElementById('impress');

const handler = function (e) {
  if (e.type === 'impress:stepleave') {
    console.log('Leaving step "' + e.target.id + '" (target hint: "' + e.detail.next.id + '")');
  } else if (e.type === 'impress:stepenter') {
    console.log('Entered step "' + e.target.id + '"');
    moreStepEnterListener(e);
    if (e.target.id === 'outro') {
      video.player.setCurrentTime(0);
      video.player.play();
    }
  }
};

impressApi.lib.gc.addEventListener(impressRoot, 'impress:stepenter', handler);
impressApi.lib.gc.addEventListener(impressRoot, 'impress:stepleave', handler);
impressApi.init();

let videoPlaying = false;
video.addEventListener('play', () => videoPlaying = true);
video.addEventListener('pause', () => videoPlaying = false);

window.addEventListener('keyup', function (e) {
  if (!document.body.classList.contains('impress-on-outro'))
    return;

  if (e.key === 's') {
    videoPlaying ? video.pause() : video.play();
  }
});

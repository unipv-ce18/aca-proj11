import './showMore.scss'

import hljs from 'highlight.js';
import 'highlight.js/styles/foundation.css'

let compRoot = undefined;
let contentOverlay = undefined;
let showMoreBtn = undefined;

let currentContent = undefined;

function initialize() {
  compRoot = document.getElementById('show-more');
  contentOverlay = document.getElementById('more-content-overlay');
  showMoreBtn = document.getElementById('show-more-btn');

  showMoreBtn.onclick = cbShowMore;
  window.addEventListener('keyup', cbKeyPress);
}

function cbShowMore() {
  if (compRoot.classList.contains('visible')) {
    compRoot.classList.remove('visible');
    return;
  }

  if (currentContent === undefined) return;

  if (contentOverlay.firstChild !== null)
    contentOverlay.removeChild(contentOverlay.firstChild);
  contentOverlay.appendChild(currentContent.cloneNode(true));
  for (let iframe of contentOverlay.querySelectorAll('iframe')) {
    iframe.onload = function () {
      iframe.contentDocument.addEventListener('keyup', cbKeyPress);
    }
  }
  for (let pre of contentOverlay.querySelectorAll('pre')) {
    hljs.highlightBlock(pre);
  }

  contentOverlay.className = '';
  contentOverlay.classList.add('extra-on-' + currentContent.parentElement.id);
  compRoot.classList.add('visible');
}

function cbKeyPress(e) {
  switch (e.key) {
    case 'a':
      cbShowMore();
      break;
    case 'Escape':
      compRoot.classList.remove('visible');
      break;
  }
}

export function stepEnterListener(e) {
  if (compRoot === undefined) initialize();

  currentContent = e.target.getElementsByClassName('additional-content')[0];
  if (currentContent === undefined)
    showMoreBtn.classList.add('hidden');
  else
    showMoreBtn.classList.remove('hidden');
}

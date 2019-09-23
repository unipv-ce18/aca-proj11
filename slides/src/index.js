import './styles/style.scss';
import 'impress.js';

import renderMathInElement from 'katex/dist/contrib/auto-render';
import 'katex/dist/katex.css';

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
impressApi.init();

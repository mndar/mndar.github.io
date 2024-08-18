/* global bootstrap: false */
(() => {
    'use strict'
    const tooltipTriggerList = Array.from(document.querySelectorAll('[data-bs-toggle="tooltip"]'))
    tooltipTriggerList.forEach(tooltipTriggerEl => {
      new bootstrap.Tooltip(tooltipTriggerEl)
    })
  })()

  function showContent(content_div) {
    //alert('Showing Content From ' + content_div);
    
    content_area = document.getElementById('content');
    content = document.getElementById(content_div).innerHTML;
    content_area.innerHTML = content;
    

  }
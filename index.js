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

  function openLink (link_name) {
    switch (link_name) {
      case "blog":
        url = "https://medium.com/@mandar.joshi.4084";
        break;
      case 'linkedin':
        url = "https://www.linkedin.com/in/mandardevlinux";
        break;
      case 'github':
        url = "https://github.com/mndar";
        break;
    }
    window.open(url, '_blank').focus();
  }
  
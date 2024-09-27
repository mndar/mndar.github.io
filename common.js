  function openLink (link_name) {
    switch (link_name) {
      case "blog":
        url = "https://mandarjoshi4084.substack.com";
        break;
      case 'linkedin':
        url = "https://www.linkedin.com/in/mandardevlinux";
        break;
      case 'github':
        url = "https://github.com/mndar";
        break;
      case 'resume':
        url = "https://mndar.github.io/resume/Mandar Joshi - Linux Developer.pdf";
        break;
      default:
        url = link_name;
    }
    window.open(url, '_blank').focus();
  }

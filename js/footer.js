function loadFooter() {
    const contents = `<hr/>
<span>Copyright (c) 2015-2022 wereturtle</span>
<span class="float-end"><a href="#">Back to top</a></span>`

    var footer = document.body.getElementsByTagName("footer")[0];

    if (null != footer) {
        footer.classList.add("container");
        footer.innerHTML = contents;
    }
}

loadFooter();
function loadNavBar() {
    const navbar = `
<nav class="navbar navbar-expand-lg m-0 p-0"> <!-- navbar-dark bg-dark -->
    <div class="container-fluid">
        <a class="navbar-brand p-0 m-0" href="index.html">
            <img src="images/ghostwriter.svg" alt="" height="24"
                class="d-inline-block align-text-center"/>
            ghostwriter
        </a>
        <button class="navbar-toggler" type="button" data-bs-toggle="collapse"
                data-bs-target="#main-navigation" aria-controls="main-navigation"
                aria-expanded="false" aria-label="Toggle navigation">
            <span class="navbar-toggler-icon"></span>
        </button>
        <div class="collapse navbar-collapse" id="main-navigation">
            <ul class="navbar-nav mr-auto">
                <li class="nav-item">
                    <a class="nav-link" href="download.html">Download</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" href="documentation.html">Documentation</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" href="contribute.html">Contribute</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" href="donate.html">Donate</a>
                </li>
            </ul>
        </div>
    </div>
</nav>`

    document.getElementById("navbar").innerHTML = navbar;

}

loadNavBar();
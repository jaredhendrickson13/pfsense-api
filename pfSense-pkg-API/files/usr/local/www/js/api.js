function toggle_advanced_settings() {
    // Variables and constants
    const BUTTON = document.getElementById("display_advanced");
    const CONTENT = document.getElementById("api-advanced-settings");

    // Toggle the display class
    CONTENT.classList.toggle("hide-api-advanced-settings");

    // Change the button text to match the requested action
    if (BUTTON.innerHTML.includes("Display")) {
        BUTTON.innerHTML = BUTTON.innerHTML.replace("Display", "Hide");
    } else {
        BUTTON.innerHTML = BUTTON.innerHTML.replace("Hide", "Display");
    }
}

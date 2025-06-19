function playMeow() {
        const audio = document.getElementById('meowSound');
        audio.play().catch(err => console.log("Audio play failed:", err));
}
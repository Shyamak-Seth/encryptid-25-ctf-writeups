function sendForReview() {
    fetch(window.location.href, {method: 'POST'})
    .then(resp => resp.text())
    .then(data => {
        alert(data)
        window.location.href = '/'
    })
    .catch(err => {
        alert("Error: " + err)
    })
}
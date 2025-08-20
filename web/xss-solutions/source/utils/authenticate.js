const passport = require('passport')

function ensureAuthenticated(req, res, next) {
    if (req.isAuthenticated()) {
      return next();
    } else res.redirect('/login');
  }

function forwardAuthenticated(req, res, next) {
    if (!req.isAuthenticated()) {
      return next();
    } else {
      res.redirect('/')
    } 
}

function ensureAdmin(req, res, next) {
    if (req.user) {
        if (req.user.admin == true) {
            return next()
        }
    }
    return res.status(403).end("Not an admin.")
}

module.exports = {ensureAuthenticated, forwardAuthenticated, ensureAdmin}
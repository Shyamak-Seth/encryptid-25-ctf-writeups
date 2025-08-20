const passportLocal = require('passport-local'),
    LocalStrategy = (passportLocal.Strategy),
    bcrypt = require('bcrypt'),
    User = require('../schemas/userSchema');

module.exports = function passportInit(passport) {
    passport.use(
        new LocalStrategy({usernameField: 'username'},
            async (username, password, done) => {
                const user = await User.findOne({username});
                if (!user) {
                    return done(null, false, { message: 'No account with this username!' });
                }
                bcrypt.compare(password, user.password, (err, isMatch) => {
                    if (err) throw err
                    if (isMatch) {
                        return done(null, user);
                    } else {
                        return done(null, false, { message: 'Password incorrect' });
                    }
                });
            })
    );

    passport.serializeUser(function (user, done) {
        done(null, user.id);
    });

    passport.deserializeUser(async function (id, done) {
        try {
            const user = await User.findById(id)
            done(null, user)
        } catch (err) {
            console.log(err)
        }
    });
}
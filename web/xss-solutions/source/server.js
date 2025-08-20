require("dotenv").config();

const express = require("express"),
  app = express(),
  PORT = process.env.PORT || 3000,
  mongoose = require("mongoose"),
  session = require('express-session'),
  flash = require('express-flash'),
  passport = require('passport'),
  passportInit = require('./utils/passport-config'),
  bcrypt = require('bcrypt'),
  {ensureAuthenticated, ensureAdmin, forwardAuthenticated} = require('./utils/authenticate')
  puppeteer = require("puppeteer");

const User = require("./schemas/userSchema"),
    Application = require('./schemas/applicationSchema');

const TAGS_BLACKLIST = ['template', 'svg', 'script', 'img', 'iframe', 'object', 'style', 'audio', 'video', 'canvas', 'math', 'form', 'button', 'input']
const ATTR_BLACKLIST = ["onabort","onafterprint","onanimationend","onanimationiteration","onanimationstart","onbeforeprint","onbeforeunload","onblur","oncanplay","oncanplaythrough","onchange","onclick","oncontextmenu","oncopy","oncut","ondblclick","ondrag","ondragend","ondragenter","ondragleave","ondragover","ondragstart","ondrop","ondurationchange","onended","onerror","onfocus","onfocusin","onfocusout","onformchange","onforminput","onhashchange","oninput","oninvalid","onkeydown","onkeypress","onkeyup","onload","onloadeddata","onloadedmetadata","onloadstart","onmessage","onmousedown","onmouseenter","onmouseleave","onmousemove","onmouseout","onmouseover","onmouseup","onmousewheel","onoffline","ononline","onpagehide","onpageshow","onpaste","onpause","onplay","onplaying","onpopstate","onprogress","onratechange","onreset","onresize","onscroll","onsearch","onseeked","onseeking","onselect","onshow","onstalled","onstorage","onsubmit","onsuspend","ontimeupdate","ontoggle","ontouchcancel","ontouchend","ontouchmove","ontouchstart","ontransitionend","onunload","onvolumechange","onwaiting","onwheel","style","src","srcdoc","action","formaction","background","poster","target","data","codebase","usemap","longdesc","contenteditable","xmlns","xlink:href"]

app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(express.static('public'))

mongoose.connect(process.env.MONGO_URI, console.log("MONGOOSE CONNECTED"));

app.set("view engine", "ejs");
app.use(flash())
app.use(session({
    secret: process.env.SESSION_SECRET,
    resave: true,
    saveUninitialized: true
}))
app.use(express.urlencoded({extended: false}))
app.use(express.json())

app.use(passport.initialize())
app.use(passport.session())
passportInit(passport)

app.get('/', (req, res) => {
    res.render('index')
})

app.get('/admin', ensureAuthenticated, ensureAdmin, (req, res) => {
    res.render('admin', {flag: process.env.FLAG})
})

app.get('/login', forwardAuthenticated, (req, res) => {
    res.render('login')
})

app.post('/login', forwardAuthenticated, passport.authenticate('local', {
    failureFlash: true,
    failureRedirect: '/login',
    successRedirect: '/'
}));

app.get('/logout', (req, res) => {
    req.logout(function(err) {
        if (err) {
            console.log(err)
            return res.status(500).end('Error');
        }
        req.session.destroy(function(err) {
        if (err) {
            console.log(err)
            return res.status(500).end('Error');
        }
        res.clearCookie('connect.sid');
        res.redirect('/login');
        });
    });
})

app.get('/register', forwardAuthenticated, (req, res) => {
    res.render('register', {error: false})
})

app.post('/register', forwardAuthenticated, async (req, res) => {
    try {
        const {username, password, cnfpassword} = req.body
        if (TAGS_BLACKLIST.includes(username.toLowerCase()) || ATTR_BLACKLIST.includes(username.toLowerCase())) {
            return res.render('register', {error: 'The username contains blacklisted phrases/words.'})
        }
        const foundUser = await User.findOne({username})
        if (foundUser) {
            return res.render('register', {error: 'The username already exists!'})
        }
        if (password != cnfpassword) {
            return res.render('register', {error: "The passwords do not match!"})
        }
        const hashed_pwd = await bcrypt.hash(password, 256)
        const newUser = new User({
            username: username,
            password: hashed_pwd
        })
        await newUser.save()
        res.redirect('/login')
    } catch (err) {
        console.log(err)
        res.status(500).send("There was an error. Please try again.")
    }
})

app.get('/apply', ensureAuthenticated, (req, res) => {
    res.render('apply')
}) 

app.post('/apply', ensureAuthenticated, async (req, res) => {
    try {
        const {title, description} = req.body
        if (!title || !description) {
            return res.render('apply', {error: 'Please provide the title and the description.'})
        }
        const newApplication = new Application({
            username: req.user.username,
            title: title,
            description: description
        })
        await newApplication.save()
        res.redirect(`/applications/${newApplication.id}`)
    } catch (err) {
        console.log(err)
        res.status(500).send("There was an error.")
    }
})

app.get('/applications/:id', ensureAuthenticated, async (req, res) => {
    try {
        const {id} = req.params
        const foundApplication = await Application.findById(id)
        if (foundApplication) {
            return res.render('application', {
                username: foundApplication.username, 
                title: foundApplication.title,
                description: foundApplication.description
            })
        } else {
            res.redirect('/')
        }
    } catch (err) {
        console.log(err)
        res.status(500).send("No such application.")
    }
})

app.post('/applications/:id', ensureAuthenticated, async (req, res) => {
    try {
        const {id} = req.params
        const foundApplication = await Application.findById(id)
        if (!foundApplication) {
            res.status(500).end('No such application found.')
        } else {
            res.end('The application will be reviewed by an admin shortly!')
            const browser = await puppeteer.launch({
                headless: true,
                executablePath: process.env.PUPPETEER_EXECUTABLE_PATH,
                args: ['--no-sandbox', '--disable-setuid-sandbox', '--disable-dev-shm-usage']
            });
            const page = await browser.newPage();
            await page.goto(
                `${process.env.SITE_URL}/login`,
                {
                    waitUntil: "networkidle0",
                }
            );
            await page.type('input[name="username"]', 'admin')
            await page.type('input[name="password"]', process.env.ADMIN_PASSWORD)
            await Promise.all([
                page.click('input[type="submit"]'),
                page.waitForNavigation({ waitUntil: 'networkidle0' }),
            ]);
            const cookies = await page.cookies();
            const dashboard = await browser.newPage()
            await dashboard.setCookie(...cookies)
            await dashboard.goto(`${process.env.SITE_URL}/applications/${id}`, {waitUntil: 'networkidle0'})
            await browser.close()
            console.log("browser ka ho gaya kaam tamaam")
        }
    } catch (err) {
        console.log(err)
        res.status(500).end('Error.')
    }
})

app.listen(PORT, console.log(`Server connected on port ${PORT}`))

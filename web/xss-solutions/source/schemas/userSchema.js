const mongoose = require('mongoose');

const reqString = {type: String, required: true}

const userSchema = new mongoose.Schema({
  username: reqString,
  password: reqString,
  admin: {
    type: Boolean,
    default: false,
    required: true
  }
});

const User = mongoose.model('User', userSchema);

module.exports = User;

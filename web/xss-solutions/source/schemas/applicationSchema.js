const mongoose = require('mongoose');

const reqString = {type: String, required: true}

const applicationSchema = new mongoose.Schema({
  title: reqString,
  description: reqString,
  username: reqString
});

const Application = mongoose.model('Application', applicationSchema);

module.exports = Application;
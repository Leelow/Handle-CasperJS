var wrapper = new Wrapper();
var exec = require('child_process').exec;

/**
 * Constructor
 * @type {Wrapper}
 */
function Wrapper() {
    this.daemonPath = undefined;
}

/**
 * Initialise with daemonPath parameter
 * @param daemonPath
 * @returns {Wrapper}
 * @private
 */
Wrapper.prototype._init = function(daemonPath) {
    this.daemonPath = daemonPath;
    return this;
};

/**
 * Get the status of the daemon
 * @public
 */
Wrapper.prototype.status = function () {

    exec(this.daemonPath + ' status', function(error, stdout, stderr) {
        console.log('stdout: ' + stdout);
        //console.log('stderr: ' + stderr);
        if (error !== null) {
            console.log('exec error: ' + error);
        }
    });

};

exports.status = function() {
    exec(this.daemonPath + ' status', function(error, stdout, stderr) {
        console.log('stdout: ' + stdout);
        //console.log('stderr: ' + stderr);
        if (error !== null) {
            console.log('exec error: ' + error);
        }
    });
}

    module.exports = function() {
    return wrapper._init("/home/leo/handle-casperjs/daemon-handle/build/daemon");
};


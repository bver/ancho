(function() {

  var Cc = Components.classes;
  var Ci = Components.interfaces;
  var Cu = Components.utils;

  Cu.import("resource://gre/modules/Services.jsm");
  Cu.import("resource://gre/modules/FileUtils.jsm");

  var StorageAPI = function(extensionState, contentWindow, storageSpace) {
    var dbFile = FileUtils.getFile('ProfD', ['ancho_storage.sqlite3']);
    this.connection = Services.storage.openDatabase(dbFile); // create the file if it does not exist
    this.tableName = extensionState.id.replace(/[^A-Za-z]/g, '_') + '_' + storageSpace; // no sanitization necessary
    this.connection.executeSimpleSQL('CREATE TABLE IF NOT EXISTS '+this.tableName+' ( key TEXT PRIMARY KEY, value TEXT )');
    // A separate CREATE INDEX command for keys not needed:
    // http://stackoverflow.com/questions/3379292/is-an-index-needed-for-a-primary-key-in-sqlite
  };

  function dbError(err) {
    throw new Error('Storage error: ' + err.message);
  }

  StorageAPI.prototype = {

    get: function(keys, callback) {
      if (keys) {
        var myKeys = [];
        var results = {};

        var callCallback = true;
        var myCallback = function(results) {
          if (callCallback && typeof callback === 'function') {
            callCallback = false;
            callback(results);
          }
        };

        if (Array.isArray(keys)) {
          myKeys = keys;
        } else if (typeof keys == 'string') {
          myKeys = [keys];
        } else if (typeof keys == 'object') {
          for (var key in keys) {
            myKeys.push(key);
            results[key] = keys[key]; // preparing defaults
          }
        } else {
          throw new Error("Invocation of get doesn't match definition get(optional string or array or object keys, function callback)");
        }

        if (myKeys.length) {
          var statement = this.connection.createStatement('SELECT key, value FROM '+this.tableName+' WHERE key IN (:key)');
          var par, params = statement.newBindingParamsArray();
          for (var i=0; i<myKeys.length; i++) {
            par = params.newBindingParams();
            par.bindByName('key', myKeys[i]);
            params.addParams(par);
          }
          statement.bindParameters(params);

          statement.executeAsync({
            handleResult: function(resultRows) {
              var key, row = resultRows.getNextRow();
              while (row) {
                key = row.getResultByName('key');
                results[key] = JSON.parse(row.getResultByName('value'));
                row = resultRows.getNextRow();
              }
              myCallback(results);
            },

            // handleResult is not called with the empty resultRows
            // so we have to call back in this case
            handleCompletion: function(reason) {
              if (reason == Ci.mozIStorageStatementCallback.REASON_FINISHED) {
                myCallback(results);
              } else {
                dbError({ message: 'select statement not finished' });
              }
            },

            handleError: dbError
          });
        } else {
          // Empty myKeys has to be handled separately otherwise statement.bindParameters fails.
          // Note that get([]) and get({}) work in Chrome without errors.
          myCallback({});
        }
      }
    },

    set: function(items, callback) {
      if (typeof items === 'object') {
        var statement = this.connection.createStatement('REPLACE INTO '+this.tableName+' (key, value) VALUES (:key, :value)');

        var par, params = statement.newBindingParamsArray();
        for (var key in items) {
          par = params.newBindingParams();
          par.bindByName('key', key);
          par.bindByName('value', JSON.stringify(items[key]));
          params.addParams(par);
        }
        statement.bindParameters(params);

        statement.executeAsync({
          handleCompletion: function(reason) {
            if (reason == Ci.mozIStorageStatementCallback.REASON_FINISHED) {
              if (typeof callback === 'function') {
                callback();
              }
            } else {
              dbError({ message: 'replace statement not finished' });
            }
          },

          handleError: dbError
        });

      } else {
        throw new Error("Invocation of set doesn't match definition set(object items, optional function callback)");
      }
    }
  };

  module.exports = StorageAPI;

}).call(this);


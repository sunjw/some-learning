const betterSqlite3 = require('better-sqlite3');

const utils = require('./utils');

class SqliteDb {

    constructor(dbPath) {
        this.dbPath = dbPath;
        this.db = null;
    }

    open() {
        if (this.db != 0) {
            return;
        }

        utils.log('SqliteDb.open, dbPath=[%s]', this.dbPath);
        this.db = new betterSqlite3(this.dbPath);
    }

    close() {
        if (!this.db) {
            return;
        }

        utils.log('SqliteDb.close');
        this.db.close();
        this.db = null;
    }

    query(sql, params, callback) {
        if (!this.db) {
            callback(new Error('No db opened.'), []);
            return;
        }

        utils.log('SqliteDb.query:\n[' + sql + ']\n@[' + params + ']');
        let stmt = this.db.prepare(sql);
        try {
            let rows = stmt.all(params);
            callback(null, rows);
        } catch (err) {
            callback(err);
        }
    }
}

// exports
exports.SqliteDb = SqliteDb;

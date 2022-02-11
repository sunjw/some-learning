const betterSqlite3 = require('better-sqlite3');

const utils = require('./utils');

class SqliteDb {

    constructor(dbPath) {
        this.dbPath = dbPath;
        this.db = null;
    }

    open() {
        if (this.db) {
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

    prepare(sql) {
        let stmt = this.db.prepare(sql);
        return stmt;
    }

    query(sql, params, callback) {
        if (!this.db) {
            callback(new Error('No db opened.'), []);
            return;
        }

        utils.log('SqliteDb.query:\n[' + sql + ']\n@[' + params + ']');
        let stmt = this.prepare(sql);
        try {
            let rows = stmt.all(params);
            callback(null, rows);
        } catch (err) {
            callback(err);
        }
    }

    exec(sql, params, callback) {
        if (!this.db) {
            callback(new Error('No db opened.'), []);
            return;
        }

        utils.log('SqliteDb.exec:\n[' + sql + ']\n@[' + params + ']');
        let stmt = this.prepare(sql);
        try {
            let info = stmt.run(params);
            callback(null, info);
        } catch (err) {
            callback(err);
        }
    }
}

// exports
module.exports = SqliteDb;

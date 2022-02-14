const betterSqlite3 = require('better-sqlite3');

const utils = require('./utils');

class SqliteDb {

    constructor(dbPath, printSql = false) {
        this.dbPath = dbPath;
        this.db = null;
        this.printSql = printSql;
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

        if (this.printSql) {
            utils.log('SqliteDb.query:\n[' + sql + ']\n@[' + params + ']');
        }
        try {
            let stmt = this.prepare(sql);
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

        if (this.printSql) {
            utils.log('SqliteDb.exec:\n[' + sql + ']\n@[' + params + ']');
        }
        try {
            let stmt = this.prepare(sql);
            let info = stmt.run(params);
            callback(null, info);
        } catch (err) {
            callback(err);
        }
    }
}

// exports
module.exports = SqliteDb;

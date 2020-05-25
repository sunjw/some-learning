package main

import (
	"github.com/cihub/seelog"
	"github.com/sirupsen/logrus"
	"os"
)

func GetLogrusJson() *logrus.Logger {
	logger := logrus.New()
	logger.SetFormatter(&logrus.JSONFormatter{})
	logger.SetReportCaller(true)
	logger.Out = os.Stdout
	logger.SetLevel(logrus.InfoLevel)
	return logger
}

var logrusJson = GetLogrusJson()

func init() {
	loggerSeelog, err := seelog.LoggerFromConfigAsFile("seelog.xml")
	if err != nil {
		panic("init seelog error")
	}
	_ = seelog.ReplaceLogger(loggerSeelog)
}

func main() {
	defer seelog.Flush()

	logrusJson.Info("main start...")
	logrusJson.Info("main end.")

	seelog.Info("main start...")
	seelog.Info("main end.")
}

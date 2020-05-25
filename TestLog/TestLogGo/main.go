package main

import (
	"github.com/sirupsen/logrus"
	"os"
)

func GetLogger() *logrus.Logger {
	logger := logrus.New()
	logger.SetFormatter(&logrus.JSONFormatter{})
	logger.SetReportCaller(true)
	logger.Out = os.Stdout
	logger.SetLevel(logrus.InfoLevel)
	return logger
}

var LoggerJson = GetLogger()

func main() {
	LoggerJson.Info("main start...")
	LoggerJson.Info("main end.")
}

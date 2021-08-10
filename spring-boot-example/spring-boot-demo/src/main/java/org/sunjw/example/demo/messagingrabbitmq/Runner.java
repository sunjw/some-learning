package org.sunjw.example.demo.messagingrabbitmq;

import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.boot.CommandLineRunner;
import org.springframework.stereotype.Component;
import org.sunjw.example.demo.DemoApplication;

import java.util.concurrent.TimeUnit;

@Component
public class Runner implements CommandLineRunner {

    private final RabbitTemplate rabbitTemplate;
    private final RabbitReceiver rabbitReceiver;

    public Runner(RabbitReceiver receiver, RabbitTemplate rabbitTemplate) {
        this.rabbitReceiver = receiver;
        this.rabbitTemplate = rabbitTemplate;
    }

    @Override
    public void run(String... args) throws Exception {
        System.out.println("Sending message...");
        rabbitTemplate.convertAndSend(DemoApplication.topicExchangeName, "foo.bar.baz", "Hello from RabbitMQ!");
        rabbitReceiver.getLatch().await(10000, TimeUnit.MILLISECONDS);
    }
}

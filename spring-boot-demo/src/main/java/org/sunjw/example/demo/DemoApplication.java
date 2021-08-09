package org.sunjw.example.demo;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.amqp.core.Binding;
import org.springframework.amqp.core.BindingBuilder;
import org.springframework.amqp.core.Queue;
import org.springframework.amqp.core.TopicExchange;
import org.springframework.amqp.rabbit.connection.ConnectionFactory;
import org.springframework.amqp.rabbit.listener.SimpleMessageListenerContainer;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.context.properties.EnableConfigurationProperties;
import org.springframework.boot.web.client.RestTemplateBuilder;
import org.springframework.context.ApplicationContext;
import org.springframework.context.annotation.Bean;
import org.springframework.data.redis.connection.RedisConnectionFactory;
import org.springframework.data.redis.core.StringRedisTemplate;
import org.springframework.data.redis.listener.PatternTopic;
import org.springframework.data.redis.listener.RedisMessageListenerContainer;
import org.springframework.data.redis.listener.adapter.MessageListenerAdapter;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.scheduling.annotation.EnableAsync;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.scheduling.concurrent.ThreadPoolTaskExecutor;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.client.RestTemplate;
import org.sunjw.example.demo.accessingdatajpa.CustomerRepository;
import org.sunjw.example.demo.accessingdatajpa.JpaCustomer;
import org.sunjw.example.demo.accessingdataneo4j.Person;
import org.sunjw.example.demo.accessingdataneo4j.PersonRepository;
import org.sunjw.example.demo.consumingrest.Quote;
import org.sunjw.example.demo.messagingrabbitmq.RabbitReceiver;
import org.sunjw.example.demo.messagingredis.Receiver;
import org.sunjw.example.demo.relationaldataaccess.Customer;
import org.sunjw.example.demo.uploadingfiles.StorageProperties;
import org.sunjw.example.demo.uploadingfiles.StorageService;

import java.util.Arrays;
import java.util.List;
import java.util.concurrent.Executor;
import java.util.stream.Collectors;

@SpringBootApplication
@EnableScheduling
@EnableAsync
@EnableConfigurationProperties(StorageProperties.class)
@RestController
public class DemoApplication implements CommandLineRunner {

    private static final Logger log = LoggerFactory.getLogger(DemoApplication.class);

    public static final String topicExchangeName = "spring-boot-exchange";
    public static final String queueName = "spring-boot";

    @Autowired
    JdbcTemplate jdbcTemplate;

    @Autowired
    private ApplicationContext context;

    public static void main(String[] args) {
        SpringApplication.run(DemoApplication.class, args);
    }

    @GetMapping("/hello")
    public String hello(@RequestParam(value = "name", defaultValue = "World") String name) {
        return String.format("Hello %s!", name);
    }

    @Bean
    public RestTemplate restTemplate(RestTemplateBuilder builder) {
        return builder.build();
    }

    @Bean
    public CommandLineRunner run(RestTemplate restTemplate) throws Exception {
        return args -> {
            Quote quote = restTemplate.getForObject(
                    "https://quoters.apps.pcfone.io/api/random", Quote.class);
            log.info(quote.toString());
        };
    }

    @Override
    public void run(String... args) throws Exception {
        log.info("Creating tables");

        jdbcTemplate.execute("DROP TABLE customers IF EXISTS");
        jdbcTemplate.execute("CREATE TABLE customers(" +
                "id SERIAL, first_name VARCHAR(255), last_name VARCHAR(255))");

        // Split up the array of whole names into an array of first/last names
        List<Object[]> splitUpNames = Arrays.asList("John Woo", "Jeff Dean", "Josh Bloch", "Josh Long").stream()
                .map(name -> name.split(" "))
                .collect(Collectors.toList());

        // Use a Java 8 stream to print out each tuple of the list
        splitUpNames.forEach(name -> log.info(String.format("Inserting customer record for %s %s", name[0], name[1])));

        // Uses JdbcTemplate's batchUpdate operation to bulk load data
        jdbcTemplate.batchUpdate("INSERT INTO customers(first_name, last_name) VALUES (?,?)", splitUpNames);

        log.info("Querying for customer records where first_name = 'Josh':");
        jdbcTemplate.query(
                "SELECT id, first_name, last_name FROM customers WHERE first_name = ?",
                (rs, rowNum) -> new Customer(rs.getLong("id"), rs.getString("first_name"), rs.getString("last_name")),
                "Josh"
        ).forEach(customer -> log.info(customer.toString()));

        StringRedisTemplate template = context.getBean(StringRedisTemplate.class);
        Receiver receiver = context.getBean(Receiver.class);

        while (receiver.getCount() == 0) {

            log.info("Sending message...");
            template.convertAndSend("chat", "Hello from Redis!");
            Thread.sleep(500L);
        }
    }

    @Bean
    CommandLineRunner init(StorageService storageService) {
        return (args) -> {
            //storageService.deleteAll();
            storageService.init();
            log.info("StorageService init");
        };
    }

    @Bean
    RedisMessageListenerContainer container(RedisConnectionFactory connectionFactory,
                                            MessageListenerAdapter listenerAdapter) {

        RedisMessageListenerContainer container = new RedisMessageListenerContainer();
        container.setConnectionFactory(connectionFactory);
        container.addMessageListener(listenerAdapter, new PatternTopic("chat"));

        return container;
    }

    @Bean
    MessageListenerAdapter listenerAdapter(Receiver receiver) {
        return new MessageListenerAdapter(receiver, "receiveMessage");
    }

    @Bean
    Receiver receiver() {
        return new Receiver();
    }

    @Bean
    StringRedisTemplate template(RedisConnectionFactory connectionFactory) {
        return new StringRedisTemplate(connectionFactory);
    }

    @Bean
    Queue queue() {
        return new Queue(queueName, false);
    }

    @Bean
    TopicExchange exchange() {
        return new TopicExchange(topicExchangeName);
    }

    @Bean
    Binding binding(Queue queue, TopicExchange exchange) {
        return BindingBuilder.bind(queue).to(exchange).with("foo.bar.#");
    }

    @Bean
    SimpleMessageListenerContainer rabbitContainer(ConnectionFactory connectionFactory,
                                                   org.springframework.amqp.rabbit.listener.adapter.MessageListenerAdapter rabbitListenerAdapter) {
        SimpleMessageListenerContainer container = new SimpleMessageListenerContainer();
        container.setConnectionFactory(connectionFactory);
        container.setQueueNames(queueName);
        container.setMessageListener(rabbitListenerAdapter);
        return container;
    }

    @Bean
    org.springframework.amqp.rabbit.listener.adapter.MessageListenerAdapter rabbitListenerAdapter(RabbitReceiver rabbitReceiver) {
        return new org.springframework.amqp.rabbit.listener.adapter.MessageListenerAdapter(rabbitReceiver, "receiveMessage");
    }

    @Bean
    CommandLineRunner neo4jDemo(PersonRepository personRepository) {
        return args -> {

            personRepository.deleteAll();

            Person greg = new Person("Greg");
            Person roy = new Person("Roy");
            Person craig = new Person("Craig");

            List<Person> team = Arrays.asList(greg, roy, craig);

            log.info("Before linking up with Neo4j...");

            team.stream().forEach(person -> log.info("\t" + person.toString()));

            personRepository.save(greg);
            personRepository.save(roy);
            personRepository.save(craig);

            greg = personRepository.findByName(greg.getName());
            greg.worksWith(roy);
            greg.worksWith(craig);
            personRepository.save(greg);

            roy = personRepository.findByName(roy.getName());
            roy.worksWith(craig);
            // We already know that roy works with greg
            personRepository.save(roy);

            // We already know craig works with roy and greg

            log.info("Lookup each person by name...");
            team.stream().forEach(person -> log.info(
                    "\t" + personRepository.findByName(person.getName()).toString()));

            List<Person> teammates = personRepository.findByTeammatesName(greg.getName());
            log.info("The following have Greg as a teammate...");
            teammates.stream().forEach(person -> log.info("\t" + person.getName()));
        };
    }

    @Bean
    public CommandLineRunner jpaDemo(CustomerRepository repository) {
        return (args) -> {
            // save a few customers
            repository.save(new JpaCustomer("Jack", "Bauer"));
            repository.save(new JpaCustomer("Chloe", "O'Brian"));
            repository.save(new JpaCustomer("Kim", "Bauer"));
            repository.save(new JpaCustomer("David", "Palmer"));
            repository.save(new JpaCustomer("Michelle", "Dessler"));

            // fetch all customers
            log.info("Customers found with findAll():");
            log.info("-------------------------------");
            for (JpaCustomer jpaCustomer : repository.findAll()) {
                log.info(jpaCustomer.toString());
            }
            log.info("");

            // fetch an individual customer by ID
            JpaCustomer jpaCustomer = repository.findById(1L);
            log.info("Customer found with findById(1L):");
            log.info("--------------------------------");
            log.info(jpaCustomer.toString());
            log.info("");

            // fetch customers by last name
            log.info("Customer found with findByLastName('Bauer'):");
            log.info("--------------------------------------------");
            repository.findByLastName("Bauer").forEach(bauer -> {
                log.info(bauer.toString());
            });
            // for (Customer bauer : repository.findByLastName("Bauer")) {
            //  log.info(bauer.toString());
            // }
            log.info("");
        };
    }

    @Bean
    public Executor taskExecutor() {
        ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
        executor.setCorePoolSize(2);
        executor.setMaxPoolSize(2);
        executor.setQueueCapacity(500);
        executor.setThreadNamePrefix("GithubLookup-");
        executor.initialize();
        return executor;
    }

}

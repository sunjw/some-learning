package org.sunjw.example.demo.asyncmethod;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.boot.CommandLineRunner;
import org.springframework.stereotype.Component;

import java.util.concurrent.CompletableFuture;

@Component
public class GitHubAppRunner implements CommandLineRunner {

    private static final Logger logger = LoggerFactory.getLogger(GitHubAppRunner.class);

    private final GitHubLookupService gitHubLookupService;

    public GitHubAppRunner(GitHubLookupService gitHubLookupService) {
        this.gitHubLookupService = gitHubLookupService;
    }

    @Override
    public void run(String... args) throws Exception {
        // Start the clock
        long start = System.currentTimeMillis();

        // Kick of multiple, asynchronous lookups
        CompletableFuture<GitHubUser> page1 = gitHubLookupService.findUser("PivotalSoftware");
        CompletableFuture<GitHubUser> page2 = gitHubLookupService.findUser("CloudFoundry");
        CompletableFuture<GitHubUser> page3 = gitHubLookupService.findUser("Spring-Projects");

        // Wait until they are all done
        CompletableFuture.allOf(page1, page2, page3).join();

        // Print results, including elapsed time
        logger.info("Elapsed time: " + (System.currentTimeMillis() - start));
        logger.info("--> " + page1.get());
        logger.info("--> " + page2.get());
        logger.info("--> " + page3.get());

    }
}

package org.sunjw.example.demo.asyncmethod;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.boot.web.client.RestTemplateBuilder;
import org.springframework.scheduling.annotation.Async;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;

import java.util.concurrent.CompletableFuture;

@Service
public class GitHubLookupService {

    private static final Logger logger = LoggerFactory.getLogger(GitHubLookupService.class);

    private final RestTemplate restTemplate;

    public GitHubLookupService(RestTemplateBuilder restTemplateBuilder) {
        this.restTemplate = restTemplateBuilder.build();
    }

    @Async
    public CompletableFuture<GitHubUser> findUser(String githubUser) throws InterruptedException {
        logger.info("Looking up " + githubUser);
        String url = String.format("https://api.github.com/users/%s", githubUser);
        GitHubUser results = new GitHubUser();
        try {
            results = restTemplate.getForObject(url, GitHubUser.class);
            // Artificial delay of 1s for demonstration purposes
            Thread.sleep(1000L);
        } catch (Exception ex) {
            logger.error("RestTemplate exception", ex);
        }
        return CompletableFuture.completedFuture(results);
    }
}

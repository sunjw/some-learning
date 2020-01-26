package org.sunjw.fragmentbestpractice;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import org.sunjw.news.ui.NewsContentFragment;

public class NewsContentActivity extends AppCompatActivity {

    private final static String NEWS_TITLE = "news_title";
    private final static String NEWS_CONTENT = "news_content";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.news_content);

        Intent intent = getIntent();
        String newsTitle = intent.getStringExtra(NEWS_TITLE);
        String newsContent = intent.getStringExtra(NEWS_CONTENT);
        NewsContentFragment newsContentFragment = (NewsContentFragment)
                getSupportFragmentManager().findFragmentById(R.id.news_content_fragment);
        newsContentFragment.refresh(newsTitle, newsContent);
    }

    public static void actionStart(Context context, String newsTitle, String newsContent) {
        Intent intent = new Intent(context, NewsContentActivity.class);
        intent.putExtra(NEWS_TITLE, newsTitle);
        intent.putExtra(NEWS_CONTENT, newsContent);
        context.startActivity(intent);
    }
}

package org.sunjw.recyclerviewtest;

import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import org.sunjw.fla.Fruit;

import java.util.List;

/**
 * Created by sunjw on 2017/2/5.
 */

public class FruitRecyclerAdapter extends RecyclerView.Adapter<FruitRecyclerAdapter.ViewHolder> {

    private List<Fruit> mFruitList;

    static class ViewHolder extends RecyclerView.ViewHolder {

        ImageView fruitImage;
        TextView fruitName;

        public ViewHolder(View itemView) {
            super(itemView);

            fruitImage = (ImageView) itemView.findViewById(R.id.fruit_image);
            fruitName = (TextView) itemView.findViewById(R.id.fruit_name);
        }

        public void setBackgroundColor(int color) {
            itemView.setBackgroundColor(color);
        }
    }

    public FruitRecyclerAdapter(List<Fruit> fruitList) {
        mFruitList = fruitList;
    }

    @Override
    public int getItemCount() {
        return mFruitList.size();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.fruit_item, parent, false);
        final ViewHolder holder = new ViewHolder(view);

        holder.itemView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int pos = holder.getAdapterPosition();
                Fruit fruit = mFruitList.get(pos);
                Toast.makeText(v.getContext(), "Clicked View - (" + fruit.getName() + ")",
                        Toast.LENGTH_SHORT).show();
            }
        });

        holder.fruitImage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int pos = holder.getAdapterPosition();
                Fruit fruit = mFruitList.get(pos);
                Toast.makeText(v.getContext(), "Clicked Image - (" + fruit.getName() + ")",
                        Toast.LENGTH_SHORT).show();
            }
        });

        return holder;
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        Fruit fruit = mFruitList.get(position);
        holder.fruitImage.setImageResource(fruit.getImageId());
        holder.fruitName.setText(fruit.getName());
        if (position % 2 == 1) {
            holder.setBackgroundColor(0xffdddddd);
        } else {
            holder.setBackgroundColor(android.R.color.white);
        }
    }
}

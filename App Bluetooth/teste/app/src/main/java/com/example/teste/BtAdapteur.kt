package com.example.teste

import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import android.widget.ImageView
import android.widget.TextView


// creat the list view with the picture
class BtAdapteur(context: Context, resource: Int, private val items: List<PostData>) : // intre element of the list view
    ArrayAdapter<PostData>(context, resource, items) {

    override fun getView(position: Int, convertView: View?, parent: ViewGroup): View {
        val inflater = LayoutInflater.from(context)
        val view = inflater.inflate(R.layout.itembtlistview, parent, false)

        val textName = view.findViewById<TextView>(R.id.textView)
        val imageIcon = view.findViewById<ImageView>(R.id.imageView)

        val item = items[position] // stoke the name tanks to the position
        textName.text = item.name // find the name of item
        imageIcon.visibility = if (item.isVisible == true) View.VISIBLE else View.GONE // pster the icon bar de connected

        return view
    }
}
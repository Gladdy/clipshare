# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
from django.conf import settings


class Migration(migrations.Migration):

    dependencies = [
        ('clipshare', '0004_delete_file'),
    ]

    operations = [
        migrations.CreateModel(
            name='File',
            fields=[
                ('id', models.CharField(max_length=16, primary_key=True, serialize=False)),
                ('filename', models.TextField()),
                ('filesize', models.IntegerField()),
                ('mimetype', models.CharField(max_length=64)),
                ('location', models.CharField(max_length=256)),
                ('url', models.CharField(max_length=256)),
            ],
        ),
        migrations.CreateModel(
            name='UserProfile',
            fields=[
                ('user', models.OneToOneField(serialize=False, to=settings.AUTH_USER_MODEL, primary_key=True)),
            ],
        ),
        migrations.AddField(
            model_name='file',
            name='user',
            field=models.ForeignKey(to=settings.AUTH_USER_MODEL),
        ),
    ]

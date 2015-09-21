# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
from django.conf import settings


class Migration(migrations.Migration):

    dependencies = [
        migrations.swappable_dependency(settings.AUTH_USER_MODEL),
    ]

    operations = [
        migrations.CreateModel(
            name='File',
            fields=[
                ('id', models.CharField(serialize=False, primary_key=True, max_length=16)),
                ('filename', models.TextField()),
                ('filesize', models.IntegerField()),
                ('mimetype', models.CharField(max_length=64)),
                ('location', models.FilePathField()),
                ('url', models.URLField()),
                ('user', models.ForeignKey(to=settings.AUTH_USER_MODEL)),
            ],
        ),
    ]

import random
import string

from django.db import IntegrityError, models

from clipshare.settings import s3, AWS_STORAGE_BUCKET_NAME, AWS_S3_ROOT


def random_id(length=8):
    return ''.join(random.choice(string.ascii_letters + string.digits) for i in range(length))


class FileManager(models.Manager):

    def store(self, user, file):

        trial = 0
        bucket = s3.Bucket(AWS_STORAGE_BUCKET_NAME)

        while trial < 10:

            identifier = random_id()
            key = None
            f = None

            try:
                # Check whether the folder already exists
                if len(list(bucket.objects.filter(Prefix=identifier).limit(1))) > 0:
                    raise IntegrityError()

                key = identifier + '/' + file.name
                url = AWS_S3_ROOT + key

                # Create a database entry for the file
                f = self.model(id=identifier,
                               user=user,
                               filename=file.name,
                               filesize=file.size,
                               mimetype=file.content_type,
                               url=url)
                f.save()

                # Actually store the file somewhere on the filesystem
                s3.Object(bucket.name, key).put(Body=file, ContentType=file.content_type)

                return f

            except IntegrityError as e:
                if key is not None:
                    s3.Object(bucket.name, key).delete()

                if f is not None:
                    f.delete()


            trial += 1

        raise IOError("Unable to store the file")